<?php
// $Id: themekey.system.inc,v 1.3 2009/09/25 12:28:33 mkalkbrenner Exp $

function themekey_system_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['system:host'] = array(
    'description' => t('System: Server Host')
  );

  return array('attributes' => $attributes);
}

function themekey_system_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['system:host'] = $_SERVER['HTTP_HOST'];

  return $parameters;
}
