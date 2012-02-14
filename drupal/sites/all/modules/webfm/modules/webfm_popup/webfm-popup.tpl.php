<?php

/**
 * @file
 * Display the webfm browser in a popup window.
 *
 * @see webfm_theme(), webfm-browser.tpl.php
 */
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">
<head>
  <title><?php print t('File Browser'); ?></title>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php print drupal_get_js('header'); ?>
</head>

<body class="webfm">
<div id="messages"><?php print theme('status_messages'); ?></div>
<?php print $content; ?>
<?php print drupal_get_js('footer'); ?>
</body>
</html>
