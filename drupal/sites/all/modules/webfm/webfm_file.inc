<?php

/**
 * @file
 * The functions in this file handle the file operations of Webfm.
 */

/**
 * webfm_mkdir -called from the ajax action - switch case 'mkdir':
 *
 * @param string $source - the source directory path
 * @param string $dest - the destination directory name
 *
 * @ret string   -name of directory if created
 *      FALSE    -if unsuccessful
 */
function webfm_mkdir($source, $dest, $rename = FALSE, &$error) {
  $mkdir_path = ($source .'/'. $dest);
  if (!is_dir($mkdir_path)) {
    if (@mkdir($mkdir_path)) {
      chmod($mkdir_path, 0775);
      if (!is_writable($mkdir_path)) {
        $error = t('The directory %path is not writable', array('%path' => $mkdir_path));
      }
      else {
        return $mkdir_path;
      }

    }
  }
  elseif ($rename == TRUE) {
    $count = 0;
    while (is_dir($munge_path = ($mkdir_path .'_'. $count))) {
      $count++;
    }

    if (@mkdir($munge_path)) {
      chmod($munge_path, 0775);
      if (!is_writable($munge_path)) {
        $error = t('The directory %path is not writable', array('%path' => $munge_path));
      }
      else {
        return $munge_path;
      }
    }
    else {
      $error = t('Creating the directory %dirname failed', array('%dirname' => $munge_path));
    }
  }
  else {
    $error = t('The directory %dirname already exists', array('%dirname' => $mkdir_path));
  }

  return FALSE;
}

/**
 * webfm_rename -called from the ajax action - switch case 'rename':
 *
 * @param string  $source - the source directory path
 * @param string  $dest - the destination directory name
 * @param bool    $uid - user id
 * @param string &$err_arr - ref to error array for client feedback
 *
 * @ret   bool   -true if the directory/file is renamed and file record(s)
 *                updated if dir/file under db control
 */
function webfm_rename($source, $dest, $uid, &$err_arr) {
  $dest_temp = $dest .'~'; //handle possible case problem
  //File
  //if target is a file and new name isn't already used...
  if (is_file($source) && !is_file($dest)) {
    $file_o = webfm_get_file_record('', $source);
    //if we are admin or owner of target file...
    if ($uid == 1 || ($file_o && ($uid == $file_o->uid)) || ($file_o && webfm_file_mod_access($file_o))) {
      //check validity of new file name
      $file = new stdClass();
      $file->filename = _webfm_basename($dest);
      $file->filepath = $source;
      $file->filesize = 0; //avoid fail for size - we're interested in extensions
      if (webfm_enum_validate($file, $err_arr) === FALSE) {
        //illegal file extension
        $err_arr[] = t('Invalid name');
        return FALSE;
      }
      //if file is not read-only...rename
      if (@rename($source, $dest_temp)) {
        if (!@rename($dest_temp, $dest)) {
          @rename($dest_temp, $source);
          $err_arr[] = t('Renaming %filename failed.', array('%filename' => _webfm_basename($source)));
          return FALSE;
        }
      }
      else {
        $err_arr[] = t('The directory %dirname is read-only.', array('%dirname' => _webfm_basename($source)));
        return FALSE;
      }
    }
    else {
      $err_arr[] = t('permission denied');
      return FALSE;
    }
    return (webfm_rename_db_file($source, $dest, $err_arr));
  }

  //Directory
  //if target is a directory, new name is a unique path and we are an admin...
  elseif (is_dir($source) && !is_dir($dest) && ($uid == 1)) {
    //if the target isn't read-only
    if (@rename($source, $dest_temp)) {
      //directory rename is OK, back out, rename db files and rename dir again
      @rename($dest_temp, $source);
      if (webfm_rename_db_dir_recur($source, $dest, TRUE, $err_arr)) {
        @rename($source, $dest_temp);
        @rename($dest_temp, $dest);
      }
      else {
        $err_arr[] = t('webfm_rename_db_dir_recur error - db may be corrupted');
        drupal_set_message(t('webfm_rename_db_dir_recur error - db may be corrupted'), 'error');
        return FALSE;
      }
      return TRUE;
    }
    else {
      $err_arr[] = t('The file %filename is read only.', array('%filename' => $source));
      return FALSE;
    }
  }

  $err_arr[] = t('Renaming from %rename_from to %rename_dest failed.', array('%rename_from' => array_pop(explode('/', $source)), '%rename_dest' => array_pop(explode('/', $dest))));

  return FALSE;
}

function webfm_rename_db_file($source, $dest, &$err_arr) {
  if ($fid = webfm_get_fid($source)) {
    if (!webfm_dbupdate_file($fid, $dest)) {
      $err_arr[] = t('Failed to update the database for the file with id %fid', array('%fid' => $fid));
      return FALSE;
    }
  }
  return TRUE;
}

function webfm_rename_db_dir_recur($source, $dest, $ret, &$err_arr) {
  if ($handle = opendir($source)) {
    while (($file = readdir($handle)) !== FALSE) {
      if ($file != '.' && $file != '..') {
        $source_item  = $source .'/'. $file;
        $dest_item = $dest .'/'. $file;
        if (is_file($source_item)) {
          if (!(webfm_rename_db_file($source_item, $dest_item, $err_arr))) {
            $ret = FALSE;
          }
        }
        elseif (is_dir($source_item)) {
          $ret = webfm_rename_db_dir_recur($source_item, $dest_item, $ret, $err_arr);
        }
      }
    }
    closedir($handle);
  }
  return $ret;
}

/**
 * webfm_delete_dir_recur - called from the ajax action - switch case 'delete':
 *
 * @param string $source - the source directory path
 * @param bool   $ret - return value seed
 * @param string &$err_arr - ref to error array for client feedback
 *
 * @ret   bool   -true if the directory or file is deleted and all file records updated
 */
function webfm_delete_dir_recur($source, $ret, &$err_arr) {
  if ($handle = opendir($source)) {
    while (($file = readdir($handle)) !== FALSE) {
      if ($file != '.' && $file != '..') {
        $source_item  = $source .'/'. $file;
        if (is_file($source_item)) {
          if (!(webfm_delete_file($source_item, $error))) {
            $err_arr[] = $error;
            $ret = FALSE;
          }
        }
        elseif (is_dir($source_item)) {
          webfm_delete_dir_recur($source_item, $ret, $err_arr);
        }
      }
    }
    closedir($handle);
  }
  else {
    $err_arr[] = t('Unable to open the directory %dirname.', array('%dirname' => $source));
    return FALSE;
  }

  if (($retn = rmdir($source)) == FALSE) {
    $err_arr[] = t('Unable to remove directory %dirname.', array('%dirname' => $source));
  }

  return ($retn == TRUE && $ret == TRUE);
}

function webfm_delete_file($source, &$error) {
  $file = webfm_get_file_record('', $source);
  if (@unlink($source)) {
    if ($file && (!webfm_dbdelete_file($file->fid))) {
      $error = t('Could not delete %filename from database.', array('%filename' => $source));
      return FALSE;
    }
  }
  elseif (file_exists($source)) {
    $error = t('The file %filename could not be deleted', array('%filename' => $source));
    return FALSE;
  }
  return TRUE;
}

/**
 * Move a directory or file and update database
 *
 * @param string  $source - the source directory path
 * @param string  $dest - the destination directory name
 * @param bool    $uid - uid of user making move request
 * @param string &$err_arr - ref to error array for client feedback
 *
 * @ret   bool   -true if any directory change (used to switch client cache)
 *
 */
function webfm_move($source, $dest, $uid, &$err_arr) {
  $dest = rtrim($dest, '/ \/ '. DIRECTORY_SEPARATOR) .'/'. _webfm_basename($source);
  return webfm_rename($source, $dest, $uid, $err_arr);
}

/**
 * webfm_insert_file - inserts a file into the webfm_file table
 *
 * @param string  $path - path relative to drupal root
 * @param string &$err_arr - ref to error array for client feedback
 *
 * @return bool - TRUE if query executed successfully, otherwise FALSE
 */
function webfm_insert_file($path, &$err) {
  if (!webfm_get_fid($path)) {
    $file = new stdClass();
    $file->filepath = $path;
    if (webfm_enum_validate($file, $err)) {
      return ($ret = webfm_dbinsert_file($file, $err));
    }
  }
  return FALSE;
}

/**
 * webfm_insert_dir - inserts files into the webfm_file table
 *
 * @param string  $path - path relative to drupal root
 * @param bool    $recur - flag to indicate if recursive operation into sub-dirs
 * @param string &$err_arr - ref to error array for client feedback
 *
 * @return obj - contains error flag and count of successful and failed inserts
 */
function webfm_insert_dir($path, $recur, &$err_arr) {
  $result = new stdClass();
  $result->cnt = 0;
  $result->errcnt = 0;
  if ($handle = opendir($path)) {
    $bl = array('.', '..', '.svn', '.htaccess');
    while (false !== ($file = readdir($handle))) {
      $file_path = $path .'/'. $file;
        if (!in_array(drupal_strtolower($file), $bl) and !is_link($file_path)) {
        if (is_file($file_path)) {
          // file
          if (webfm_insert_file($file_path, $err)) {
            $result->cnt++;
          }
          elseif ($err) {
            // If file already in db then return webfm_insert_file returns false without error
            $err_arr[] = $err;
            $result->errcnt++;
          }
        }
        elseif ($recur && is_dir($file_path)) {
          $nest_result = webfm_insert_dir($file_path, $recur, $err_arr);
          $result->cnt += $nest_result->cnt;
          $result->errcnt += $nest_result->errcnt;
        }
      }
    }
    closedir($handle);
  }
  else {
    $err_arr[] = t('Could not open the directory %dirname', array('%dirname' => $path));
  }

  return $result;
}

/**
 * Get file's basename (locale independent)
 *
 *
 * @param string Filename
 *
 * @return string Basename
 */
function _webfm_basename($filename) {
  //Originally by Aleksander Machniak
  //http://pear.php.net/bugs/bug.php?id=14529&edit=12&patch=mime.patch&revision=1222363721
  //basename() is not unicode safe and locale dependent
  if (stristr(PHP_OS, 'win') || stristr(PHP_OS, 'netware')) {
    return preg_replace('/^.*[\\\\\\/]/', '', $filename);
  }
  else {
    return preg_replace('/^.*[\/]/', '', $filename);
  }
}

/**
 * Rawurldecode fails if the filename is utf8
 * Inspired by http://weierophinney.net/matthew/archives/133-PHP-decoding-of-Javascript-encodeURIComponent-values.html
 *
 * @param string|array $value UTF8 encoded by encodeURIComponent()
 */
function _webfm_utf8urldecode($value) {
    if (is_array($value)) {
        foreach ($value as $key => $val) {
            $value[$key] = _webfm_utf8urldecode($val);
        }
    }
    else {
        $value = (string)$value;
        $value = preg_replace('/%([0-9a-f]{2})/ie', "chr(hexdec('\\1'))", $value);
    }
    return $value;
}