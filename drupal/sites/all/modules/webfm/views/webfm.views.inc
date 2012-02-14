<?php

/**
 * @file
 * The views parts of Webfm..
 *
 * It implements the hooks to display webfm in views.
 */

/**
 * Implementation of hook_views_handlers().
 *
 * Provides Handlers and Path to Views
 *
 * @return
 *    array() with the webfm views path and the the supported handlers.
 *
 */
function webfm_views_handlers() {
  return array(
    'info' => array(
    'path' => drupal_get_path('module', 'webfm') .'/views',
  ),
    'handlers' => array(
  // Fields
      'webfm_views_handler_file_name' => array(
        'parent' => 'views_handler_field',
  ),
      'webfm_views_handler_file_meta' => array(
        'parent' => 'views_handler_field',
  ),
  )
  );
}

/**
 * Implementation of hook_views_data().
 *
 * Describes fields and kind of data in the fields to views.
 *
 * @return
 *    array() With this data.
 *
 */
function webfm_views_data() {
  $data = array();

  /**
   * The webfm_attach table links directly to the node table,
   * via the nid field
   */
  $data['webfm_attach']['table']['group'] = t('WebFM Attachments');
  $data['webfm_attach']['table']['join']['node'] = array(
    'left_field' => 'nid',
    'field' => 'nid',
  );
  $data['webfm_attach']['table']['join']['webfm_file'] = array(
    'left_field' => 'fid',
    'field' => 'fid',
  );
  $data['webfm_attach']['nid'] = array(
    'title' => t('Node ID'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
      'validate type' => 'nid',
  ),
    'sort' => array('handler' => 'views_handler_sort'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'field' => 'nid',
      'label' => t('node'),
  ),
  );
  $data['webfm_attach']['fid'] = array(
    'title' => t('File ID'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
      'validate type' => 'fid',
  ),
    'sort' => array('handler' => 'views_handler_sort'),
  );
  $data['webfm_file']['table']['group'] = t('WebFM Files');
  $data['webfm_file']['table']['base'] = array(
    'field' => 'fid',
    'title' => t('WebFM Files')
  );
  $data['webfm_file']['table']['join']['user'] = array(
    'left_field' => 'uid',
    'field' => 'uid',
  );
  $data['webfm_file']['fid'] = array(
    'title' => t('File ID'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
      'validate type' => 'fid',
  ),
    'sort' => array('handler' => 'views_handler_sort'),
  );
  $data['webfm_file']['uid'] = array(
    'title' => t('User ID'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'numeric' => TRUE,
      'validate type' => 'uid',
  ),
    'sort' => array('handler' => 'views_handler_sort'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'relationship field' => 'uid',
      'label' => t('user'),
  ),
  );
  $data['webfm_file']['file_name'] = array(
    'title' => t('File by Name/Title.'),
    'field' => array(
      'handler' => 'webfm_views_handler_file_name',
      'option' => array(
        '#type' => 'select',
        '#options' => array(
          'link' => t('With links'),
          'nolink' => t('Without links'))),
      'click sortable' => TRUE,
  ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display a the file name or title. Metadata title is used where present when \'Display metadata title\' checked in settings.'),
      );
      $data['webfm_file']['fpath'] = array(
    'title' => t('File path'),
    'field' => array(
      'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the path of the file')
      );
      $data['webfm_file']['fsize'] = array(
    'title' => t('File Size'),
    'field' => array(
      'handler' => 'views_handler_field_file_size',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the size of the file')
      );
      $data['webfm_file']['fmime'] = array(
    'title' => t('Mime/Type'),
    'field' => array(
      'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the mime/type of the file')
      );
      $data['webfm_file']['fcreatedate'] = array(
    'title' => t('File Creation Date'),
    'field' => array(
      'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort_date'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_date'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_date'
      ),
    'help' => t('Display the date of files that have been attached to a node via WebFM')
      );
      $data['webfm_file']['ftitle'] = array(
    'title' => t('Title'),
    'field' => array(
      'handler' => 'webfm_views_handler_file_meta',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the file title of files that have been attached to a node via WebFM (with or without links to the files themselves)')
      );
      $data['webfm_file']['fdesc'] = array(
    'title' => t('File Description'),
    'field' => array(
      'handler' => 'webfm_views_handler_file_meta',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the description of files that have been attached to a node via WebFM')
      );
      $data['webfm_file']['flang'] = array(
    'title' => t('File Language'),
    'field' => array(
      'handler' => 'webfm_views_handler_file_meta',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the language of files that have been attached to a node via WebFM')
      );
      $data['webfm_file']['fpublisher'] = array(
    'title' => t('File Publisher'),
    'field' => array(
      'handler' => 'webfm_views_handler_file_meta',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the publisher of files that have been attached to a node via WebFM')
      );
      $data['webfm_file']['dl_cnt'] = array(
    'title' => t('Download Count'),
    'field' => array(
      'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
    'sort' => array(
      'handler' => 'views_handler_sort'
      ),
    'filter' => array(
      'handler' => 'views_handler_filter_string'
      ),
    'argument' => array(
      'handler' => 'views_handler_argument_string'
      ),
    'help' => t('Display the publisher of files that have been attached to a node via WebFM')
      );
      return $data;
}