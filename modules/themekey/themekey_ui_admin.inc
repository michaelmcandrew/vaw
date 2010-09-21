<?php
// $Id: themekey_ui_admin.inc,v 1.5 2009/11/09 20:02:51 mkalkbrenner Exp $

require_once(drupal_get_path('module', 'themekey_ui') .'/themekey_ui_helper.inc');

/**
 * Function _themekey_ui_settings_form().
 */
function _themekey_ui_settings_form() {
  $form['themekey_ui'] = array(
    '#type' => 'fieldset',
    '#title' => t('UI Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );

  if (module_exists('path')) {
    $form['themekey_ui']['themekey_ui_pathalias'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show theme option in the \'URL aliases\' administration'),
      '#default_value' => variable_get('themekey_ui_pathalias', 0),
      '#description' => t('Assign themes to paths/path aliases from the \'URL aliases\' administration pages.')
    );
  }
  //
  $nodeform = variable_get('themekey_ui_nodeform', 0);
  $form['themekey_ui']['themekey_ui_nodeform'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show theme option in create/edit node forms'),
    '#default_value' => $nodeform,
    '#description' => t('Assign themes from create/edit node forms. This will show a \'Theme\' section on create/edit node pages.')
  );
  if ($nodeform) {
    $form['themekey_ui']['content_type'] = array(
      '#type' => 'fieldset',
      '#title' => t('Content Types'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE
    );
    $form['themekey_ui']['content_type'] = array(
      '#type' => 'fieldset',
      '#title' => t('Show \'Theme\' option for nodes of type'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['themekey_ui']['content_type']['table'] = array(
      '#theme' => 'themekey_ui_table',
      '#header' => array(t('Content Type'), t('Enabled'))
    );
    foreach (node_get_types('names') as $type => $title) {
      $form['themekey_ui']['content_type']['table'][$type]['title'] = array('#value' => $title);
      $form['themekey_ui']['content_type']['table'][$type]['themekey_ui_nodeform|'. $type] = array(
        '#type' => 'checkbox',
        '#default_value' => variable_get('themekey_ui_nodeform|'. $type, 1),
      );
    }
  }

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('_themekey_ui_settings_submit')
  );

  return $form;
}

/**
 * Function _themekey_ui_settings_submit().
 */
function _themekey_ui_settings_submit($form, &$form_state) {
  //
  foreach ($form_state['values'] as $key => $value) {
    $pos = strpos($key, 'themekey_ui_');
    if ($pos !== FALSE && $pos == 0) {
      variable_set($key, $value);
    }
  }

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Function _themekey_ui_nodeform().
 */
function _themekey_ui_nodeform(&$form, &$form_state) {
  $theme = !empty($form['#node']->theme) ? $form['#node']->theme : 'default';

  $form['nodeform_theme'] = array(
    '#type' => 'fieldset',
    '#title' => t('Theme'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE
  );

  $form['nodeform_theme']['theme'] = array(
    '#type' => 'select',
    '#default_value' => $theme,
    '#options' => _themekey_theme_options(),
    '#description' => t('Theme to be assigned to current node. If you select "System Default" ThemeKey applies his rules for paths and properties.'),
  );

  if (!_themekey_check_theme_enabled($theme, TRUE)) {
    $form['nodeform_theme']['theme']['#attributes'] = array('class' => 'error');
  }
}

/**
 * Function _themekey_ui_pathalias().
 */
function _themekey_ui_pathalias(&$form, &$form_state) {
  if (!isset($form['#alias'])) {
    return;
  }

  $pathalias_form['pathalias_theme']['theme'] = array(
    '#type' => 'select',
    '#title' => t('Theme'),
    '#default_value' => themekey_ui_get_path_theme($form['#alias']['src']),
    '#options' => _themekey_theme_options(),
    '#description' => t('Theme to be assigned to current path'),
  );
  $pathalias_form['pathalias_theme']['themekey_ui_submit'] = array(
    '#type' => 'value',
    '#value' => '_themekey_ui_pathalias_submit'
  );
  array_splice($form, 4, 0, $pathalias_form);
  array_unshift($form['#submit'], 'themekey_ui_form_submit');
}

/**
 * Function _themekey_ui_pathalias_submit().
 */
function _themekey_ui_pathalias_submit($form, &$form_state) {
  if (!isset($form_state['values']['theme'])) {
    themekey_ui_del_path_theme($form['#alias']['src']);
  }
  else {
    themekey_ui_set_path_theme($form['#alias']['src'], $form_state['values']['theme']);
  }
}

/**
 * Function theme_themekey_ui_table().
 */
function theme_themekey_ui_table($form) {
  $header = isset($form['#header']) ? $form['#header'] : array();
  $attributes = isset($form['#attributes']) ? $form['#attributes'] : array();

  $rows = array();
  foreach (element_children($form) as $key) {
    $row = array();
    foreach (element_children($form[$key]) as $item) {
      $row[] = drupal_render($form[$key][$item]);
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $message = check_plain(isset($form['#empty']) ? $form['#empty'] : t('There are no items in the table.'));
    $rows[] = array(array('data' => $message, 'colspan' => count($header), 'align' => 'center', 'class' => 'message'));
  }

  return count($rows) ? theme('table', $header, $rows, $attributes) : '';
}
