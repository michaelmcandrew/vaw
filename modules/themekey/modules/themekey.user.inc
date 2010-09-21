<?php
// $Id: themekey.user.inc,v 1.3 2009/09/25 12:28:33 mkalkbrenner Exp $

function themekey_user_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['user:hostname'] = array(
    'description' => t('User: Hostname')
  );
  $attributes['user:language'] = array(
    'description' => t('User: Language')
  );
  $attributes['user:name'] = array(
    'description' => t('User: Name')
  );
  $attributes['user:uid'] = array(
    'description' => t('User: ID')
  );

  return array('attributes' => $attributes);
}

function themekey_user_themekey_global() {
  global $user;

  $parameters = array();
  $parameters['user:hostname'] = isset($user->hostname) ? $user->hostname : '';
  $parameters['user:language'] = isset($user->language) ? $user->language : 'en';
  $parameters['user:name'] = isset($user->name) ? $user->name : '';
  $parameters['user:uid'] = $user->uid;

  return $parameters;
}
