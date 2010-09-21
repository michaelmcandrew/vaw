<?php
// $Id: themekey.node.inc,v 1.8 2009/11/11 13:14:11 mkalkbrenner Exp $

function themekey_node_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['node:changed'] = array('description' => t('Node: Changed date'), 'path' => 'changed');
  $attributes['node:created'] = array('description' => t('Node: Created date'), 'path' => 'created');
  $attributes['node:language'] = array('description' => t('Node: Language'), 'path' => 'language');
  $attributes['node:nid'] = array('description' => t('Node: ID'), 'path' => 'nid');
  $attributes['node:promote'] = array('description' => t('Node: Promoted'), 'path' => 'promote');
  $attributes['node:sticky'] = array('description' => t('Node: Sticky'), 'path' => 'sticky');
  $attributes['node:type'] = array('description' => t('Node: Type'), 'path' => 'type');
  $attributes['node:uid'] = array('description' => t('Node: User ID'), 'path' => 'uid');
  $attributes['node:title'] = array('description' => t('Node: Title'), 'path' => 'title');

  return array('attributes' => $attributes);
}

function themekey_node_themekey_paths() {
  $paths = array();
  $paths[] = array('path'       => 'node/#node:nid',
                   'callbacks'  => array('_themekey_node_callback'));

  return $paths;
}

function _themekey_node_callback(&$item, &$parameters) {
  if ($node = node_load($parameters['node:nid'])) {
    $theme = !empty($node->theme) ? $node->theme : 'default';

    if ('default' != $theme) {
      $parameters['node:theme'] = $theme;
      return;
    }

    _themekey_node_prepare_node($node, $parameters);
    $parameters['node:theme'] = $theme;
  }
}

/**
 * Function _themekey_node_prepare_node().
 */
function _themekey_node_prepare_node($node, &$parameters) {
  $object_array = (array)drupal_clone($node);
  $properties = variable_get('themekey_properties', array());
  foreach ($properties as $property => $details) {
    if (($value = _themekey_node_get_node_property($object_array, $details['path'])) != NULL) {
      $parameters[$property] = $value;
    }
  }
}

/**
 * Function _themekey_node_get_node_property().
 */
function _themekey_node_get_node_property($object_array, $path) {
  $parts = explode('/', $path);
  foreach ($parts as $part) {
    if (is_array($object_array) && isset($object_array[$part])) {
      $object_array = $object_array[$part];
    }
    else {
      return NULL;
    }
  }

  return is_array($object_array) ? array_keys($object_array) : $object_array;
}

