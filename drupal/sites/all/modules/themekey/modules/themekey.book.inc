<?php
// $Id: themekey.book.inc,v 1.5 2009/11/11 13:03:48 mkalkbrenner Exp $

function themekey_book_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['book:bid'] = array('description' => t('Book: ID'));
  $attributes['book:has_children'] = array('description' => t('Book: Has Children'));

  $maps = array();
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'book:bid',
                  'callback'  => 'themekey_book_nid2bid');
  $maps[] = array('src'       => 'node:nid',
                  'dst'       => 'book:has_children',
                  'callback'  => 'themekey_book_nid2has_children');

  return array('attributes' => $attributes, 'maps' => $maps);
}

function themekey_book_nid2bid($nid, $object = NULL) {
  // FIXME This function gets called up to 5 times
  // because themekey_init() calls _themekey_match_paths() 2 times
  // and _themekey_match_paths() calls _themekey_match_properties() 2 times and $callback($item, $parameters);
  if ($node = node_load($nid)) {
    if (!empty($node->book['bid'])) {
      return $node->book['bid'];
    }
  }

  return NULL;
}

function themekey_book_nid2has_children($nid, $object = NULL) {
  if ($node = node_load($nid)) {
    if (!empty($node->book['has_children'])) {
      return $node->book['has_children'];
    }
  }

  return NULL;
}
