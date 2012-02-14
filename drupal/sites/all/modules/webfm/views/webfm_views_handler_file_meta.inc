<?php

/**
 * @file
 * The Webfm views handler that provides the Metadata.
 */

class webfm_views_handler_file_meta extends views_handler_field {

  function render($values) {
    $value = $values->{$this->field_alias};
    if ($this->options['mode'] == 'link') {
      return l(rawurldecode($value), 'webfm_send/'. $values->fid);
    }
    return check_plain(rawurldecode($value));
  }

  function options_definitions() {
    $options['label'] = array('default' => $this->definition['title'], 'translatable' => TRUE);
    $options['mode'] = array('default' => 'link');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('Label'),
      '#default_value' => isset($this->options['label']) ? $this->options['label'] : '',
      '#description' => t('The label for this field that will be displayed to end users if the style requires it.'),
    );
    $form['mode'] = array(
      '#type' => 'select',
      '#title' => t('Output Mode'),
      '#options' => array(
         'link' => t('With links'),
         'nolink' => t('Without links')
    )
    );
  }
}