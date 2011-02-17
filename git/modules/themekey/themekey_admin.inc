<?php
// $Id: themekey_admin.inc,v 1.17 2009/11/12 10:36:08 mkalkbrenner Exp $

require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');

define('THEMEKEY_PAGER_LENGTH', 25);

/**
 * Function _themekey_paths_form().
 */
function _themekey_paths_form() {

  _themekey_admin_theme_warning();

  $form['paths'] = array(
    '#type' => 'fieldset',
    '#title' => t('Paths'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t(
'Here you can map themes to Drupal paths or path aliases. To add a new map entry to the table,
enter the path into the \'Path\' field and select a theme from the \'Theme\' dropdown. For example:
Path = \'node/add\' and Theme = \'Bluemarine\' switches the theme of the \'Create content\' page to
Bluemarine. You may also use wildcard characters in the path and/or specify \'Conditions\' to match.<br />
Wildcard characters are \'#\' for numeric parts and \'%\' for all characters. To match conditions
against a certain part, use an identifier with the wildcard. For example: Path =
\'comment/reply/#xyz\' matches all paths with \'comment/reply\' and a numeric third argument.
You can then specify conditions for every wildcard argument using the following syntax:
\'id=value;id2=value2;...\', e.g. \'xyz=123\'. Supported operators are \'=\' (equal), \'!\' (not equal),
\'<\' (smaller), \'>\' (greater) and \'~\' (regex match).'),
  );

  $form['paths']['table'] = array(
    '#theme' => 'themekey_table',
    '#header' => array(t('Path'), t('Conditions'), t('Theme')),
    '#tree' => TRUE
  );

  $themes = _themekey_theme_options();
  $paths = _themekey_load_paths(THEMEKEY_PAGER_LENGTH);
  foreach ($paths as $path) {
    $form['paths']['table'][$path['id']]['path'] = array(
      '#type' => 'textfield',
      '#default_value' => $path['path'],
      '#size' => 40,
      '#maxlength' => 255
    );
    $form['paths']['table'][$path['id']]['conditions'] = array(
      '#type' => 'textfield',
      '#default_value' => $path['conditions'],
      '#size' => 20,
      '#maxlength' => 255
    );
    $form['paths']['table'][$path['id']]['theme'] = array(
      '#type' => 'select',
      '#default_value' => $path['theme'],
      '#options' => $themes
    );

    if (!_themekey_check_theme_enabled($path['theme'], TRUE)) {
      $form['paths']['table'][$path['id']]['theme']['#attributes'] = array('class' => 'error');
    }
  }

  if (count($paths)) {
    $form['paths']['pager'] = array('#value' => theme('pager', array(), THEMEKEY_PAGER_LENGTH));
  }

  $form['paths']['addtable'] = array(
    '#theme' => 'themekey_table',
    '#header' => array(t('Path'), t('Conditions'), t('Theme')),
    '#tree' => TRUE
  );
  $form['paths']['addtable']['add']['path'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 40,
    '#maxlength' => 255
  );
  $form['paths']['addtable']['add']['conditions'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 20,
    '#maxlength' => 255
  );
  $form['paths']['addtable']['add']['theme'] = array(
    '#type' => 'select',
    '#default_value' => 'default',
    '#options' => $themes
  );

  return _themekey_admin_form($form, '_themekey_paths');
}

function _themekey_paths_form_validate($form, &$form_state) {
  foreach ($form_state['values']['table'] as $key => $value) {
    foreach ($form_state['values']['table'] as $key_2 => $value_2) {
      if ($key_2 == $key) {
        continue;
      }

      if (!empty($value['path']) && $value_2['path'] == $value['path'] && $value_2['conditions'] == $value['conditions']) {
        form_set_error('table][' . $key . '][path', t('You entered two identical paths with identical conditions'));
        break(2);
      }
    }
  }

  foreach ($form_state['values']['table'] as $key => $value) {
    if (!empty($value['path']) && $form_state['values']['addtable']['add']['path'] == $value['path'] && $form_state['values']['addtable']['add']['conditions'] == $value['conditions']) {
      form_set_error('addtable][add][path', t('You entered two identical paths with identical conditions'));
      break;
    }
  }
}

/**
 * Function _themekey_paths_submit().
 */
function _themekey_paths_submit($form, &$form_state) {
  if (isset($form_state['values']['table'])) {
    foreach ($form_state['values']['table'] as $id => $item) {
      if (empty($item['path'])) {
        _themekey_path_del($id);
      }
      else {
        $item['id'] = $id;
        _themekey_path_set($item);
      }
    }
  }
  //
  if (!empty($form_state['values']['addtable']['add']['path'])) {
    _themekey_path_set($form_state['values']['addtable']['add']);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Function _themekey_paths_reset().
 */
function _themekey_paths_reset($form, &$form_state) {
  _themekey_path_clear();
  drupal_set_message(t('The configuration has been reset to defaults.'));
}

/**
 * Function _themekey_properties_form().
 */
function _themekey_properties_form() {

  _themekey_admin_theme_warning();

  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#description' => t(
'Here you can map themes to object properties, e.g. to node properties such as the NodeID (nid).
To add a new map entry to the table, select a property from the \'Property\' dropdown and enter
the value which you want to assign the theme to. For example: Property = \'nid\', Value = \'123\'
and Theme = \'Bluemarine\' switches the theme of node 123 to Bluemarine. You may also specify
\'Additional Conditions\' so that the theme only applies when all conditions are matched. \'Additional
Conditions\' must be provided using the following syntax: \'property=value;property2=value2;...\'.
Supported operators are \'=\' (equal), \'!\' (not equal), \'<\' (smaller), \'>\' (greater) and \'~\' (regex match).'),
  );

  $form['properties']['table'] = array(
    '#theme' => 'themekey_table',
    '#header' => array(t('Property'), t('Value'), t('Additional Conditions'), t('Theme')),
    '#tree' => TRUE
  );

  $themes = _themekey_theme_options();
  $properties = array_keys(variable_get('themekey_properties', array()));
  if (count($properties)) {
    $config = array();
    foreach ($properties as $property) {
      $config[$property] = $property;
    }
    ksort($config);
    $properties = _themekey_load_properties(THEMEKEY_PAGER_LENGTH, FALSE);
    foreach ($properties as $property) {
      $form['properties']['table'][$property['id']]['property'] = array(
        '#type' => 'select',
        '#title' => '',
        '#default_value' => $property['property'],
        '#options' => $config
      );
      $form['properties']['table'][$property['id']]['value'] = array(
        '#type' => 'textfield',
        '#default_value' => $property['value'],
        '#size' => 25,
        '#maxlength' => 255
      );
      $form['properties']['table'][$property['id']]['conditions'] = array(
        '#type' => 'textfield',
        '#default_value' => $property['conditions'],
        '#size' => 35,
        '#maxlength' => 255
      );
      $form['properties']['table'][$property['id']]['theme'] = array(
        '#type' => 'select',
        '#default_value' => $property['theme'],
        '#options' => $themes,
      );

      if (!_themekey_check_theme_enabled($property['theme'], TRUE)) {
        $form['properties']['table'][$property['id']]['theme']['#attributes'] = array('class' => 'error');
      }
    }

    if (count($properties)) {
      $form['properties']['pager'] = array('#value' => theme('pager', array(), THEMEKEY_PAGER_LENGTH));
    }

    $form['properties']['addtable'] = array(
      '#theme' => 'themekey_table',
      '#header' => array(t('Property'), t('Value'), t('Additional Conditions'), t('Theme')),
      '#tree' => TRUE
    );
    $form['properties']['addtable']['add']['property'] = array(
      '#type' => 'select',
      '#title' => '',
      '#default_value' => '',
      '#options' => $config
    );
    $form['properties']['addtable']['add']['value'] = array(
      '#type' => 'textfield',
      '#default_value' => '',
      '#size' => 25,
      '#maxlength' => 255
    );
    $form['properties']['addtable']['add']['conditions'] = array(
      '#type' => 'textfield',
      '#default_value' => '',
      '#size' => 35,
      '#maxlength' => 255
    );
    $form['properties']['addtable']['add']['theme'] = array(
      '#type' => 'select',
      '#title' => '',
      '#default_value' => 'default',
      '#options' => $themes
    );
  }
  else {
    $message = t('Please visit the <a href="@url">ThemeKey settings</a> tab first and enable at least one
                  property for selection', array('@url' => url('admin/settings/themekey/settings')));
    drupal_set_message($message, 'error');
  }

  return _themekey_admin_form($form, '_themekey_properties');
}


function _themekey_properties_form_validate($form, &$form_state) {
  if (!empty($form_state['values']['table'])) {
    foreach ($form_state['values']['table'] as $key => $value) {
      if (!empty($value['conditions'])) {
        $conditions = _themekey_properties_explode_conditions($value['conditions']);
        if (empty($conditions) || count($conditions) != (substr_count($value['conditions'], ';') + 1)) {
          form_set_error('table][' . $key . '][conditions', t('You entered malformatted conditions'));
        }
      }

      foreach ($form_state['values']['table'] as $key_2 => $value_2) {
        if ($key_2 == $key) {
          continue;
        }

        if (!empty($value['value']) && $value_2['property'] == $value['property'] && $value_2['value'] == $value['value'] && $value_2['conditions'] == $value['conditions']) {
          form_set_error('table][' . $key . '][value', t('You entered two identical values with identical conditions for the same property'));
          break(2);
        }
      }
    }

    foreach ($form_state['values']['table'] as $key => $value) {
      if (!empty($value['value']) && $form_state['values']['addtable']['add']['property'] == $value['property'] && $form_state['values']['addtable']['add']['value'] == $value['value'] && $form_state['values']['addtable']['add']['conditions'] == $value['conditions']) {
        form_set_error('addtable][add][value', t('You entered two identical values with identical conditions for the same property'));
        break;
      }
    }
  }
}


/**
 * Function _themekey_properties_submit().
 */
function _themekey_properties_submit($form, &$form_state) {
  if (isset($form_state['values']['table'])) {
    foreach ($form_state['values']['table'] as $id => $item) {
      if (empty($item['value'])) {
        _themekey_properties_del($id);
      }
      else {
        $item['id'] = $id;
        _themekey_properties_set($item);
      }
    }
  }
  //
  if (!empty($form_state['values']['addtable']['add']['value'])) {
    _themekey_properties_set($form_state['values']['addtable']['add']);
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Function _themekey_properties_reset().
 */
function _themekey_properties_reset($form, &$form_state) {
  _themekey_properties_clear();
  drupal_set_message(t('The configuration has been reset to defaults.'));
}

/**
 * Function _themekey_settings_form().
 */
function _themekey_settings_form() {
  // REVIEW rebuild is called to many times
  _themekey_rebuild();

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );

  $form['settings']['themekey_allthemes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide all themes for selection'),
    '#default_value' => variable_get('themekey_allthemes', 1),
    '#description' => t('Make all installed themes available for selection, not enabled ones only.')
  );

  $form['settings']['themekey_theme_maintain'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retain the theme until a new theme is set'),
    '#default_value' => variable_get('themekey_theme_maintain', 0),
    '#description' => t('Select this option to have users stay in the same theme until they
                         browse to a new page with an explicit theme set.')
  );

  if (module_exists('forum')) {
    $form['settings']['themekey_module_forum_triggers_taxonomy_vid'] = array(
      '#type' => 'checkbox',
      '#title' => t('Forum pages trigger property taxonomy:vid'),
      '#default_value' => variable_get('themekey_module_forum_triggers_taxonomy_vid', 0),
      '#description' => t('Property taxonomy:vid is set when a single node is shown (p.e. /node/17). If this option is selected, forum pages like /forum/28 will set taxonomy:vid as well.')
    );
  }

// TODO Integration of Taxonomy Menu outdated. See http://drupal.org/node/616946
//  if (module_exists('taxonomy_menu')) {
//    $form['settings']['themekey_module_taxonomy_menu_triggers_taxonomy_tid'] = array(
//      '#type' => 'checkbox',
//      '#title' => t('Module <em>Taxonomy Menu</em> triggers property taxonomy:tid'),
//      '#default_value' => variable_get('themekey_module_taxonomy_menu_triggers_taxonomy_tid', 0),
//      '#description' => t('Property taxonomy:tid is set when a single node is shown (p.e. /node/17). If this option is selected, pages created by module Taxonomy Menu like /category/31 will set taxonomy:tid as well.')
//    );
//  }

  $form['settings']['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );

  $form['settings']['properties']['table'] = array(
    '#theme' => 'themekey_table',
    '#header' => array(t('Property'), t('Description'), t('Enabled'), t('Weight'), ''),
    '#tree' => TRUE
  );

  $config = variable_get('themekey_properties', array());
  $attributes = variable_get('themekey_attributes', array());
  $properties = _themekey_properties_discover();
  foreach ($properties as $property => $path) {
    $properties[$property] = array('path' => $path,
                                   'property' => $property,
                                   'enabled' => isset($config[$property]),
                                   'weight' => isset($config[$property]) ? $config[$property]['weight']: 0);
  }
  //
  uasort($properties, '_themekey_properties_cmp');
  foreach ($properties as $property => $details) {
    $form['settings']['properties']['table'][$property]['property'] = array(
      '#value' => $property
    );
    $form['settings']['properties']['table'][$property]['description'] = array(
      '#value' => isset($attributes[$property]['description']) ? $attributes[$property]['description'] : '-',
    );
    $form['settings']['properties']['table'][$property]['enabled'] = array(
      '#type' => 'checkbox',
      '#title' => '',
      '#default_value' => $details['enabled']
    );
    $form['settings']['properties']['table'][$property]['weight'] = array(
      '#type' => 'weight',
      '#title' => '',
      '#default_value' => $details['weight']
    );
    $form['settings']['properties']['table'][$property]['path'] = array(
      '#type' => 'hidden',
      '#value' => $details['path']
    );
  }

  return _themekey_admin_form($form, '_themekey_settings', FALSE);
}

function _themekey_settings_form_validate($form, &$form_state) {
}

/**
 * Function _themekey_settings_submit().
 */
function _themekey_settings_submit($form, &$form_state) {
  $properties = array();
  if (isset($form_state['values']['table'])) {
    foreach ($form_state['values']['table'] as $property => $details) {
      if ($details['enabled']) {
        $properties[$property] = array('path' => $details['path'], 'weight' => $details['weight']);
      }
    }
    unset($form_state['values']['table']);
  }
  //
  $properties_old = variable_get('themekey_properties', array());
  $properties_removed = array_diff(array_keys($properties_old), array_keys($properties));
  _themekey_properties_delall($properties_removed);
  //
  uasort($properties, '_themekey_properties_cmp');
  variable_set('themekey_properties', $properties);
  //
  foreach ($form_state['values'] as $key => $value) {
    $pos = strpos($key, 'themekey_');
    if ($pos !== FALSE && $pos == 0) {
      variable_set($key, $value);
    }
  }

  drupal_set_message(t('The configuration options have been saved.'));
  if (count($properties) > 10) {
    drupal_set_message(t('Attention! Do not enable too many properties as this may slow down page loads.'), 'error');
  }
}

/**
 * Function _themekey_admin_form().
 */
function _themekey_admin_form($form, $base = NULL, $reset = TRUE) {
  // Default buttons
  if (isset($base)) {
    $form['buttons']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
      '#submit' => array($base .'_submit')
    );
    if ($reset) {
      $form['buttons']['reset'] = array(
        '#type' => 'submit',
        '#value' => t('Reset to defaults'),
        '#submit' => array($base .'_reset')
      );
    }
  }

  return $form;
}

/**
 * Function theme_themekey_table().
 */
function theme_themekey_table($form) {
  $rows = array();
  $header = isset($form['#header']) ? $form['#header'] : array();
  $attributes = isset($form['#attributes']) ? $form['#attributes'] : array();
  $tabledrag = isset($form['#tabledrag']) ? $form['#tabledrag'] : FALSE;

  if ($tabledrag && isset($attributes['id'])) {
    drupal_add_tabledrag($attributes['id'], 'order', 'sibling', $attributes['id'] .'-'. $tabledrag);
  }

  if (isset($attributes['description'])) {
    $rows[] = array(array('data' => $attributes['description'], 'colspan' => count($header), 'class' => 'message'));
    unset($attributes['description']);
  }

  foreach (element_children($form) as $key) {
    $row = array();
    foreach (element_children($form[$key]) as $item) {
      if ($tabledrag && $tabledrag == $item) {
        $form[$key][$item]['#attributes'] = array('class' => $attributes['id'] .'-'. $tabledrag);
      }
      $row[] = drupal_render($form[$key][$item]);
    }
    $rows[] = $tabledrag ? array('data' => $row, 'class' => 'draggable') : $row;
  }

  if (empty($rows)) {
    $message = check_plain(isset($form['#empty']) ? $form['#empty'] : t('There are no items in the table.'));
    $rows[] = array(array('data' => $message, 'colspan' => count($header), 'align' => 'center', 'class' => 'message'));
  }

  return count($rows) ? theme('table', $header, $rows, $attributes) : '';
}

function _themekey_admin_theme_warning() {
  if (variable_get('admin_theme', '0')) {
    drupal_set_message(t('Note: %admin_theme is configured as administration theme at !link. This setting is more powerful than a corresponding ThemeKey rule.',
      array('%admin_theme' => variable_get('admin_theme', '0'), '!link' => l('admin/settings/admin', 'admin/settings/admin'))), 'warning');
    if (variable_get('node_admin_theme', '0')) {
      drupal_set_message(t('Note: As configured at !link adding or editing a node will use the administration theme %admin_theme.',
        array('%admin_theme' => variable_get('admin_theme', '0'), '!link' => l('admin/settings/admin', 'admin/settings/admin'))), 'warning');
    }
  }
}
