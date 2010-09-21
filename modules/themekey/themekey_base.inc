<?php
// $Id: themekey_base.inc,v 1.14 2009/11/11 13:14:10 mkalkbrenner Exp $

/**
 * Function _themekey_invoke_modules().
 */
function _themekey_invoke_modules($hook) {
  $return = array();
  foreach (variable_get('themekey_modules', array('themekey.node')) as $module) {
    if (is_readable(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc')) {
      require_once(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc');
    }
    $function = str_replace('.', '_', $module) .'_'. $hook;
    if (function_exists($function)) {
      $return = array_merge_recursive($return, $function());
    }
  }

  return $return;
}

/* Path-based */

/**
 * Function _themekey_get_path_ancestors().
 * (based on menu_get_ancestors() in includes/menu.inc)
 */
function _themekey_get_path_ancestors($path) {
  static $path_ancestors = array();

  if (empty($path_ancestors[$path])) {
    $parts = explode('/', $path, MENU_MAX_PARTS);

    $placeholders = array();
    $ancestors = array();

    $number_parts = count($parts);
    $length =  $number_parts - 1;
    $end = (1 << $number_parts) - 1;

    $masks = variable_get('menu_masks', array());
    foreach ($masks as $i) {
      if ($i > $end) {
        continue;
      }
      elseif ($i < (1 << $length)) {
        --$length;
      }
      $current = '';
      for ($j = $length; $j >= 0; $j--) {
        if ($i & (1 << $j)) {
          $current .= $parts[$length - $j];
        }
        else {
          $current .= is_numeric($parts[$length - $j]) ? '#' : '%';
        }
        if ($j) {
          $current .= '/';
        }
      }
      $placeholders[] = "'%s'";
      $ancestors[] = $current;
      if (stristr($current, '#') !== FALSE) {
        $placeholders[] = "'%s'";
        $ancestors[] = str_replace('#', '%', $current);
      }
    }
    $path_ancestors[$path] = array($ancestors, $placeholders);
  }

  return $path_ancestors[$path];
}

/**
 * Function _themekey_match_paths().
 */
function _themekey_get_global_parameters() {
  static $global_parameters = NULL;

  if (is_null($global_parameters)) {
    $global_parameters = module_invoke_all('themekey_global');

    list($ancestors, $placeholders) = _themekey_get_path_ancestors( $_GET['q']);

    // TODO don't store non custom paths in database
    $result = db_query('SELECT * FROM {themekey_paths} WHERE path IN ('. implode(',', $placeholders) .') AND custom = 0 ORDER BY fit DESC, weight DESC', $ancestors);
    while ($item = db_fetch_array($result)) {
      $wildcards = unserialize($item['wildcards']);
      foreach ($wildcards as $index => $wildcard) {
        $global_parameters[$wildcard] = arg($index, $path);
      }

      $callbacks = unserialize($item['callbacks']);
      if (count($callbacks)) {
        foreach ($callbacks as $callback) {
          $callback($item, $global_parameters);
        }
      }
    }
  }

  return $global_parameters;
}

/**
 * Function _themekey_match_paths().
 */
function _themekey_match_paths($path) {
  list($ancestors, $placeholders) = _themekey_get_path_ancestors($path);

  $result = db_query('SELECT * FROM {themekey_paths} WHERE path IN ('. implode(',', $placeholders) .') ORDER BY fit DESC, weight DESC, custom DESC', $ancestors);
  while ($item = db_fetch_array($result)) {
    $parameters = _themekey_get_global_parameters();
    $conditions = unserialize($item['conditions']);
    //
    $wildcards = unserialize($item['wildcards']);
    foreach ($wildcards as $index => $wildcard) {
      $parameters[$wildcard] = arg($index, $path);
    }
    //
    if (!_themekey_match_conditions($conditions, $parameters)) {
      continue;
    }

    if (!_themekey_check_theme_enabled($item['theme'])) {
      continue;
    }

    return $item['theme'];
  }

  return NULL;
}

/**
 * Function _themekey_match_properties().
 */
function _themekey_match_properties() {
  $parameters = _themekey_get_global_parameters();
  $properties = variable_get('themekey_properties', array());
  foreach (array_keys($properties) as $property) {
    $value = _themekey_property_value($parameters, $property);
    if (!empty($value) || count($value)) {
      $query = 'SELECT * FROM {themekey_properties} WHERE property = \''. $property .'\' AND value ';
      $query .= is_array($value) ? ('IN ('. db_placeholders($value, 'varchar') .') ORDER BY weight') : '= \'%s\'';
      $result = db_query($query, $value);
      while ($item = db_fetch_array($result)) {
        //
        $item['conditions'] = unserialize($item['conditions']);
        if (_themekey_match_conditions($item['conditions'], $parameters)) {
          if (!_themekey_check_theme_enabled($item['theme'])) {
            continue;
          }
          //
          $callbacks = unserialize($item['callbacks']);
          if (is_array($callbacks) && count($callbacks)) {
            foreach ($callbacks as $callback) {
              $callback($item, $parameters);
            }
          }
          //
          return $item['theme'];
        }
      }
    }
  }

  return NULL;
}

/**
 * Function _themekey_match_conditions().
 */
function _themekey_match_conditions($conditions, $parameters) {
  if (is_array($conditions) && count($conditions)) {
    foreach ($conditions as $condition) {
      // Default operator is 'equal'
      if (!isset($condition['operator'])) {
        $condition['operator'] = '=';
      }

      $value = _themekey_property_value($parameters, $condition['property']);

      if (is_array($value)) {
        foreach ($value as $single_value) {
          // Supported operators for condition check:
          // smaller ('<'), greater ('>'), equal ('='), not equal ('!'), regex match ('~')
          if ($condition['operator'] == '<' && $single_value < $condition['value']) {
            return TRUE;
          }
          else if ($condition['operator'] == '>' && $single_value > $condition['value']) {
            return TRUE;
          }
          else if ($condition['operator'] == '=' && $single_value == $condition['value']) {
            return TRUE;
          }
          else if ($condition['operator'] == '!' && $single_value != $condition['value']) {
            return TRUE;
          }
          else if ($condition['operator'] == '~' && preg_match($condition['value'], $single_value)) {
            return TRUE;
          }
        }
        return FALSE;
      }
      else {
        // Supported operators for condition check:
        // smaller ('<'), greater ('>'), equal ('='), not equal ('!'), regex match ('~')
        if ($condition['operator'] == '<' && $value >= $condition['value']) {
          return FALSE;
        }
        else if ($condition['operator'] == '>' && $value <= $condition['value']) {
          return FALSE;
        }
        else if ($condition['operator'] == '=' && $value != $condition['value']) {
          return FALSE;
        }
        else if ($condition['operator'] == '!' && $value == $condition['value']) {
          return FALSE;
        }
        else if ($condition['operator'] == '~' && !preg_match($condition['value'], $value)) {
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

/**
 * Function _themekey_property_value().
 */
function _themekey_property_value(&$parameters, $property) {
  // Property value is available directly
  if (isset($parameters[$property])) {
    return $parameters[$property];
  }

  $parameters[$property] = NULL;

  $src_candidates = array();
  $maps = variable_get('themekey_maps', array());

  foreach ($maps as $pos => $map) {
    if ($map['dst'] == $property) {
      if (!empty($parameters[$map['src']])) {
        $map_func = $map['callback'];
        $parameters[$property] = $map_func($parameters[$map['src']], $parameters);
        break;
      }
      $src_candidates[$pos] = $map['src'];
    }
  }

  if (is_null($parameters[$property]) && !empty($src_candidates)) {
    foreach ($src_candidates as $pos => $src) {
      $return = _themekey_property_value($parameters, $src);
      if ($return) {
        $map_func = $maps[$pos]['callback'];
        $parameters[$property] = $map_func($return, $parameters);
        break;
      }
    }
  }

  return $parameters[$property];
}


function _themekey_check_theme_enabled($theme, $settings_page = FALSE) {
  static $themes_enabled = array();
  static $warned = FALSE;
  static $displayed_error = FALSE;

  if (!$theme || 'default' == $theme) {
    return TRUE;
  }

  if (empty($themes_enabled)) {
    if ($result = db_query("SELECT name FROM {system} WHERE type = 'theme' AND status = 1;")) {
      while ($row = db_fetch_array($result)) {
        $themes_enabled[] = $row['name'];
      }
    }
  }

  if (in_array($theme, $themes_enabled)) {
    return TRUE;
  }

  if ($settings_page) {
    if (!$displayed_error) {
      drupal_set_message(t("Your current configuration of theme rules uses at least one theme that is not enabled. Nevertheless this configuration is stored but affected rules won't be applied until the targeted theme will be enabled at !build_themes.",
        array('!build_themes' => l('admin/build/themes', 'admin/build/themes'))), 'error');
      $displayed_error = TRUE;
    }
  }
  else {
    global $user;

    if (!$warned && 1 == $user->uid) {
      drupal_set_message(t('A matching ThemeKey rule to select theme %theme was not applied because this theme is disabled. You can enable this theme at !build_themes or remove this ThemeKey rule at !themekey_paths or !themekey_properties or edit current node if the theme was selected using ThemeKey UI.',
        array('%theme' => $theme, '!build_themes' => l('admin/build/themes', 'admin/build/themes'), '!themekey_paths' => l('admin/settings/themekey', 'admin/settings/themekey'), '!themekey_properties' => l('admin/settings/themekey/properties', 'admin/settings/themekey/properties'))), 'warning');
      $warned = TRUE;
    }
  }

  return FALSE;
}