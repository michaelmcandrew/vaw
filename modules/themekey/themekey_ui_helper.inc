<?php
// $Id: themekey_ui_helper.inc,v 1.3 2009/11/09 20:02:51 mkalkbrenner Exp $

require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');

/**
 * Function themekey_ui_get_path_theme().
 */
function themekey_ui_get_path_theme($path) {
  return db_result(db_query('SELECT theme FROM {themekey_paths} WHERE path = \'%s\'', $path));
}

/**
 * Function themekey_ui_set_path_theme().
 */
function themekey_ui_set_path_theme($path, $theme = 'default') {
  $item = array('path' => $path, 'theme' => $theme);
  if ($id = db_result(db_query('SELECT id FROM {themekey_paths} WHERE path = \'%s\'', $path))) {
    $item['id'] = $id;
  }
  _themekey_path_set($item);
}

/**
 * Function themekey_ui_del_path_theme().
 */
function themekey_ui_del_path_theme($path) {
  db_query('DELETE FROM {themekey_paths} WHERE path = \'%s\'', $path);
}
