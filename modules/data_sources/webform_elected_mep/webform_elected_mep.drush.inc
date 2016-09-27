<?php

/**
 * Implementation of hook_drush_command().
 *
 * @See drush_parse_command() for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function webform_elected_mep_drush_command() {
  return array(
    'mep-import' => array(
      'description' => 'Import MEP CSV into webform_elected_mep table, overwriting everything in there.',
      'aliases' => array('mep-i'),
      'arguments' => array('csv-file' => 'The file name of the CSV file to import.',),
    ),
  );
}

/**
 * Import MEP CSV.
 */
function drush_webform_elected_mep_mep_import($csv_file_name) {
  if (! is_readable($csv_file_name)) {
    return drush_set_error(dt('Could not find the file. Please give an absolute path or arelative to the Drupal ROOT directory.'));
  }
  $data = webform_elected_mep_get_csv_data($csv_file_name);
  db_query('TRUNCATE {webform_elected_mep}');
  foreach ($data as $row) {
    if (empty($row['constituency'])) {
      $row['constituency'] = $row['country'];
    }

    try {
      drupal_write_record('webform_elected_mep', $row);
    }
    catch (Exception $e) {
      var_dump($e->getMessage());
      var_dump($row);
      continue;
      return;
    }
  }
}

function webform_elected_mep_get_csv_data($csv_file_name) {
  // $extra_trim = html_entity_decode('&nbsp;', ENT_QUOTES, 'UTF-8');
  $lines = file($csv_file_name);
  foreach ($lines as & $line) {
    $line = str_getcsv(trim($line), ',', '"', '\\');
    // $line = array_map(function($value) use($extra_trim) {return trim(trim($value), $extra_trim);}, $line);
    $line = array_map('webform_elected_mep_mb_trim', $line);
  }
  $header = array_shift($lines);
  foreach ($lines as & $line) {
    $line = array_combine($header, $line);
  }
  $name_mapping = webform_elected_mep_get_csv_header_name_mapping();
  $data = array();
  foreach ($lines as & $line) { // $line needs to be by reference because it has been in the previous foreach loops. Otherwise some row duplication weirdness occurs.
    $row = array();
    foreach ($name_mapping as $source_name => $dest_name) {
      $row[$dest_name] = isset($line[$source_name]) ? $line[$source_name] : '';
    }
    $data[] = $row;
  }
  return $data;
}

function webform_elected_mep_mb_trim($string) {
  return preg_replace('/(^\s+)|(\s+$)/us', '', $string);
}

function webform_elected_mep_get_csv_header_name_mapping() {
  return array_filter(array(
    // 'Addresses/Brussels/Phone' => 'phone',
    // 'Addresses/Strasbourg/Phone' => 'phone',
    'Country' => 'country',
    'Constituency' => 'constituency',
    'Constituencies/0/end' => NULL,
    'Facebook/0' => NULL,
    'Mail/0' => 'email',
    'Name/family' => 'last_name',
    'Name/sur' => 'first_name',
    'Photo' => NULL,
    'Twitter/0' => NULL,
    'UserID' => 'unique_id',
    'meta/updated' => NULL,
    'meta/url' => NULL,
  ));
}
