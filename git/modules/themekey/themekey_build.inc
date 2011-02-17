<?php
// $Id: themekey_build.inc,v 1.12 2009/11/12 10:36:08 mkalkbrenner Exp $

/**
 * Function _themekey_theme_options().
 */
function _themekey_theme_options($default = TRUE) {
  $themes = list_themes();
  ksort($themes);

  $options_themes = array();
  if ($default) {
    $options_themes['default'] = t('System default');
  }
  foreach ($themes as $themex) {
    if ($themex->status || variable_get('themekey_allthemes', 1)) {
      $options_themes[$themex->name] = $themex->info['name'];
    }
  }

  return $options_themes;
}

/**
 * Function _themekey_rebuild().
 */
function _themekey_rebuild() {
  // Create a list of modules in the /modules subfolder (internal modules)
  _themekey_scan_modules();
  // Get property definitions (from internal and other modules)
  $properties = module_invoke_all('themekey_properties');
  // Attributes
  $attributes = isset($properties['attributes']) ? $properties['attributes'] : array();
  drupal_alter('themekey_attributes', $attributes);
  variable_set('themekey_attributes', $attributes);
  // Property maps
  $maps = isset($properties['maps']) ? $properties['maps'] : array();
  drupal_alter('themekey_maps', $maps);
  variable_set('themekey_maps', $maps);

  // Find all previous registered paths
  $obsolete_paths = array();
  $result = db_query('SELECT id, path FROM {themekey_paths} WHERE custom = 0');
  while ($item = db_fetch_array($result)) {
    $obsolete_paths[$item['id']] = $item['path'];
  }
  // Get (and register) paths from themekey modules
  $paths = module_invoke_all('themekey_paths');
  drupal_alter('themekey_paths', $paths);
  foreach ($paths as $item) {
    _themekey_path_set($item, FALSE);
    if (isset($item['id'])) {
      unset($obsolete_paths[$item['id']]);
    }
  }
  // Unregister obsolete paths
  foreach (array_keys($obsolete_paths) as $id) {
    _themekey_path_del($id);
  }
}

/**
 * Function _themekey_scan_modules().
 */
function _themekey_scan_modules() {
  $modules = array();
  $files = file_scan_directory(drupal_get_path('module', 'themekey') .'/modules', '\themekey.[a-z]+.inc$');
  foreach ($files as $file) {
    list( , $module) = explode('.', $file->name);
    if (module_exists($module)) {
      $modules[] = $file->name;
    }
  }

  variable_set('themekey_modules', $modules);
}

/**
 * Function _themekey_include_modules().
 */
function _themekey_include_modules() {
  foreach (variable_get('themekey_modules', array('themekey.node')) as $module) {
    if (is_readable(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc')) {
      require_once(drupal_get_path('module', 'themekey') .'/modules/'. $module .'.inc');
    }
  }
}

/* Path-based */

/**
 * Function _themekey_load_paths().
 */
function _themekey_load_paths($limit = NULL) {
  $paths = array();
  $query = 'SELECT * FROM {themekey_paths} WHERE custom = 1';
  $result = isset($limit) ? pager_query($query, $limit) : db_query($query);
  while ($item = db_fetch_array($result)) {
    $item['wildcards'] = unserialize($item['wildcards']);
    if (count($item['wildcards'])) {
      $parts = explode('/', $item['path'], MENU_MAX_PARTS);
      foreach ($item['wildcards'] as $index => $wildcard) {
        $parts[$index] .= $wildcard;
      }
      $item['path'] = implode('/', $parts);
    }
    $item['conditions'] = _themekey_properties_implode_conditions(unserialize($item['conditions']));
    $paths[] = $item;
  }

  return $paths;
}

/**
 * Function _themekey_path_set().
 */
function _themekey_path_set(&$item, $custom = TRUE) {
  $item['conditions'] = (isset($item['conditions']) && !empty($item['conditions'])) ? $item['conditions'] : array();
  $item['conditions'] = _themekey_properties_explode_conditions($item['conditions']);
  $item['callbacks'] = (isset($item['callbacks']) && !empty($item['callbacks'])) ? $item['callbacks'] : array();

  list($item['fit'], $item['weight'], $item['wildcards']) = _themekey_prepare_path($item['path']);
  $item['custom'] = $custom;

  drupal_write_record('themekey_paths', $item, isset($item['id']) ? 'id' : array());
}

/**
 * Function _themekey_path_del().
 */
function _themekey_path_del($id) {
  db_query('DELETE FROM {themekey_paths} WHERE id = %d', $id);
}

/**
 * Function _themekey_path_clear().
 */
function _themekey_path_clear() {
  db_query('DELETE FROM {themekey_paths} WHERE custom = 1');
}

/**
 * Function themekey_num_paths().
 */
function _themekey_num_paths() {
  return db_result(db_query('SELECT COUNT(id) FROM {themekey_paths} WHERE custom = 1'));
}

/**
 * Function _themekey_prepare_path().
 * (based on _menu_router_build() in includes/menu.inc)
 */
function _themekey_prepare_path(&$path) {
  $fit = 0;
  $weight = 0;
  $wildcards = array();

  $parts = explode('/', $path, MENU_MAX_PARTS);
  $slashes = count($parts) - 1;
  foreach ($parts as $index => $part) {
    if (preg_match('/^(\%|\#)([a-z0-9_:]*)$/', $part, $matches)) {
      $parts[$index] = $matches[1];
      if (!empty($matches[2])) {
        $wildcards[$index] = $matches[2];
      }
      if ($matches[1] == '#') {
        $weight |=  1 << ($slashes - $index);
      }
    }
    else {
      $fit |=  1 << ($slashes - $index);
    }
  }
  $path = implode('/', $parts);

  return array($fit, $weight, $wildcards);
}

/**
 * Function _themekey_properties_discover().
 */
function _themekey_properties_discover() {
  $properties = array();

  // Module-exposed properties
  $attributes = variable_get('themekey_attributes', array());
  foreach ($attributes as $property => $details) {
    if (isset($details['path'])) {
      $properties[$property] = $details['path'];
    }
    else {
      $properties[$property] = FALSE;
    }
  }
  // URL argument properties
  // REVIEW why is this required?
  $result = db_query('SELECT wildcards FROM {themekey_paths} WHERE custom = 0');
  while ($item = db_fetch_array($result)) {
    $item['wildcards'] = unserialize($item['wildcards']);
    foreach ($item['wildcards'] as $wildcard) {
      if (!array_key_exists($wildcard, $properties)) {
        $properties[$wildcard] = $wildcard;
      }
    }
  }

  return $properties;
}

/**
 * Function _themekey_properties_explode_conditions().
 */
function _themekey_properties_explode_conditions($conditions) {
  if (!is_array($conditions)) {
    $parts = array_filter(explode(';', $conditions));
    $conditions = array();
    foreach ($parts as $part) {
      $part = trim($part);
      if (preg_match('/(.*?)([<>=!~])(.*)/', $part, $matches)) {
        $conditions[] = array(
          'property' => trim($matches[1]),
          'operator' => trim($matches[2]),
          'value' => trim($matches[3])
        );
      }
    }
  }

  return $conditions;
}

/**
 * Function _themekey_properties_implode_conditions().
 */
function _themekey_properties_implode_conditions($conditions) {
  $string = array();
  foreach ($conditions as $condition) {
    $string[] = $condition['property'] . $condition['operator'] . $condition['value'];
  }
  $string = implode('; ', $string);

  return $string;
}

/**
 * Function _themekey_load_properties().
 */
function _themekey_load_properties($limit = NULL, $order = TRUE) {
  $properties = array();
  $query = 'SELECT * FROM {themekey_properties}';
  $query .= $order ? ' ORDER BY weight' : '';
  $result = isset($limit) ? pager_query($query, $limit) : db_query($query);
  while ($item = db_fetch_array($result)) {
    $item['conditions'] = _themekey_properties_implode_conditions(unserialize($item['conditions']));
    $properties[] = $item;
  }

  return $properties;
}

/**
 * Function _themekey_properties_set().
 */
function _themekey_properties_set(&$item) {
  if (preg_match('/'. $item['property'] .'=(.*)/', $item['value'], $matches)) {
    $item['value'] = $matches[1];
  }
  $item['conditions'] = (isset($item['conditions']) && !empty($item['conditions'])) ? $item['conditions'] : array();
  $item['conditions'] = _themekey_properties_explode_conditions($item['conditions']);
  $item['callbacks'] = (isset($item['callbacks']) && !empty($item['callbacks'])) ? $item['callbacks'] : array();
  //
  $attributes = variable_get('themekey_attributes', array());
  if (isset($attributes[$item['property']]['multiple']) && $attributes[$item['property']]['multiple']) {
    _themekey_include_modules();
    $weightfn = $attributes[$item['property']]['weight'];
    $item['weight'] = function_exists($weightfn) ? $weightfn($item) : 0;
  }

  drupal_write_record('themekey_properties', $item, isset($item['id']) ? 'id' : array());
}

/**
 * Function _themekey_properties_del().
 */
function _themekey_properties_del($id) {
  db_query('DELETE FROM {themekey_properties} WHERE id = %d', $id);
}

/**
 * Function _themekey_properties_delall().
 */
function _themekey_properties_delall($properties) {
  $properties = is_array($properties) ? $properties : array($properties);
  foreach ($properties as $property) {
    db_query('DELETE FROM {themekey_properties} WHERE property = \'%s\'', $property);
  }
}

/**
 * Function _themekey_properties_clear().
 */
function _themekey_properties_clear() {
  db_query('DELETE FROM {themekey_properties}');
}

/**
 * Function _themekey_properties_cmp().
 */
function _themekey_properties_cmp($a, $b) {
  if ($a['weight'] == $b['weight']) {
    if (isset($a['property']) && isset($b['property'])) {
      return ($a['property'] < $b['property']) ? -1 : 1;
    }
    else {
      return ($a['path'] < $b['path']) ? -1 : 1;
    }
  }

  return ($a['weight'] < $b['weight']) ? -1 : 1;
}

/**
 * Function _themekey_properties_resort().
 */
function _themekey_properties_resort($key) {
  $attributes = variable_get('themekey_attributes', array());
  if (isset($attributes[$key]['multiple']) && $attributes[$key]['multiple']) {
    _themekey_include_modules();
    $weightfn = $attributes[$key]['weight'];
    $result = db_query('SELECT * FROM {themekey_properties} WHERE property = \'%s\'', $key);
    while ($item = db_fetch_array($result)) {
      $item['weight'] = function_exists($weightfn) ? $weightfn($item) : 0;
      drupal_write_record('themekey_properties', $item, 'id');
    }
  }
}
