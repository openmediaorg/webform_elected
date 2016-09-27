<?php

/**
 * @file
 *   MEP Constituency webform component.
 */

/**
 * Implements _webform_defaults_component().
 *
 * Specify the default properties of the component.
 *
 * @return
 *   An array defining the default structure of a component.
 */
function _webform_defaults_mep_constituency() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'mandatory' => 0,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'analysis' => TRUE,
      'title_display' => 0,
      'description' => '',
      'private' => FALSE,

      // elected specific configuration
      // 'data_source_type' => '',
      // 'data_source' => array(
      //   'key_component_cid' => '',
      //   'settings' => array(),
      // ),

      // 'multiple' => 0,
      // 'autoselect_hide' => 0,

      'fallback_country_name' => '',

      // Quack like a select component.
      'aslist' => 0,
      'multiple' => 1,
      'optrand' => 0,

      'other_option' => 0,
    ),
  );
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_mep_constituency($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_mep_constituency',
    '#theme_wrappers' => array('html' == $format ? 'webform_element' : 'webform_element_text'),
    '#format' => $format,
    '#value' => _webform_elected_mep_constituency_values_to_string($component, $value),
    '#component' => $component,
  );
}

/**
 * Implements _webform_edit_component().
 *
 * @param $component
 *   A Webform component array.
 * @return
 *   An array of form items to be displayed on the edit component page.
 */
function _webform_edit_mep_constituency($component) {
  $form = array();

  $form['mep_constituency'] =  array(
    '#type' => 'fieldset',
    '#title' => t('MEP Country / Constituency Configuration'),
    '#weight' => 1,
    '#collapsible' => TRUE,
  );

  $fallback_country_options = webform_elected_mep_get_countries();
  // Make sure the "Other" option is sorted at the end.
  if (isset($fallback_country_options['Other'])) {
    $other_option = $fallback_country_options['Other'];
    unset($fallback_country_options['Other']);
    $fallback_country_options['Other'] = $other_option;
  }
  $form['mep_constituency']['fallback_country_name'] = array(
    '#title'         => t('Fallback Country'),
    '#type'          => 'select',
    '#options'       => $fallback_country_options,
    '#default_value' => $component['extra']['fallback_country_name'],
    '#description'   => t('In case the IP-Address-to-Country lookup does not result in a European country or has no results, default to this country.'),
    '#required'      => TRUE,
    '#parents' => array('extra', 'fallback_country_name'),
    '#weight' => 0,
  );

  // $data_source_type_options = array('' => t('- Please choose a data source type -'));
  // $data_source_type_infos = webform_elected_get_data_source_type_info();
  // foreach ($data_source_type_infos as $type => $info) {
  //   $data_source_type_options[$type] = $info['title'];
  // }
  // 
  // $form['elected']['data_source_type'] = array(
  //   '#title'         => t('Data Source Type'),
  //   '#type'          => 'select',
  //   '#options'       => $data_source_type_options,
  //   '#default_value' => $component['extra']['data_source_type'],
  //   '#description'   => t('From what type of data source to find the elected officials'),
  //   '#required'      => TRUE,
  //   '#parents' => array('extra', 'data_source_type'),
  //   '#weight' => 0,
  //   '#ajax' => array(
  //     'callback' => '_webform_edit_elected_ajax_callback',
  //     'event' => 'change',
  //   ),
  // );
  // 
  // $form['elected']['data_source'] = array(
  //   '#type' => 'fieldset',
  //   '#title' => t('Data Source Settings'),
  //   '#weight' => 1,
  //   '#collapsible' => TRUE,
  //   '#prefix' => '<div id="webform-elected-data-source-settings">',
  //   '#suffix' => '</div>',
  // );
  // 
  // // The data source settings are added in @see webform_elected_form_webform_component_edit_form_alter().
  // 
  // $form['elected']['autoselect_hide'] = array(
  //   '#title'         => t('Auto-Select and Hide'),
  //   '#type'          => 'checkbox',
  //   '#default_value' => $component['extra']['autoselect_hide'],
  //   '#description'   => t('Enabling this option will automatically select the matching elected official(s) and hide the component from the user. If "Allow Multiple Selections" is enabled, all elected officials will be selected.  If "Allow Mutiple Selections" is disabled, the first matching elected official will be auto-selected.'),
  //   '#parents' => array('extra', 'autoselect_hide'),
  //   '#weight' => 3,
  // );

  return $form;
}

/**
 *
 */
function _webform_render_mep_constituency($component, $value = NULL, $filter = TRUE) {
  global $language;
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  if (isset($value[0]) && is_string($value[0])) {
    list($country_default, $dummy) = explode('--', $value[0], 2);
    $constituency_default = $value[0];
  }
  else {
    $country_default = webform_elected_mep_get_default_country($component['extra']['fallback_country_name']);
    $constituency_default = NULL;
  }

  $country_options = webform_elected_mep_get_countries();

  // Hack: Re-use core's translations of country names;
  if ($language->language != 'en' && function_exists('locale')) {
    foreach ($country_options as $country_option_value => & $country_option_label) {
      // Do not use t() because we translate a variable value.
      $country_option_label = locale($country_option_label);
    }
  }
  natcasesort($country_options);
  // Make sure the "Other" option is sorted at the end.
  if (isset($country_options['Other'])) {
    $other_option = $country_options['Other'];
    unset($country_options['Other']);
    $country_options['Other'] = $other_option;
  }

  $country_element = array(
    '#type' => 'select',
    '#options' => $country_options,
    '#value' => $country_default,
    '#attributes' => array(
      'class' => array(
        'webform-elected-mep-countries',
      ),
    ),
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#required' => $component['mandatory'],
    '#pre_render' => array('webform_elected_mep_pre_render_cleanup'),
  );

  $constituency_options = array();
  $constituencies_by_country = webform_elected_mep_get_constituencies();
  foreach ($constituencies_by_country as $country => $constituencies) {
    $constituency_options = array_merge($constituency_options, $constituencies);
  }

  $element = array(
    '#type' => 'select',
    '#options' => $constituency_options,
    '#weight' => $component['weight'],
    '#theme_wrappers' => array('webform_element'),
    '#prefix' => '<div class="webform-elected-mep">' . drupal_render($country_element),
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array(
        'webform-elected-mep-constituencies',
      ),
    ),
  );


  // Handle disabling.
  if (isset($component['extra']['disabled']) && $component['extra']['disabled']) {
    if ($filter) {
      $element['#attributes']['readonly'] = 'readonly';
    }
    else {
      $element['#disabled'] = TRUE;
    }
  }

  $js_settings = array(
    'countries' => $country_options,
    'constituencies' => $constituencies_by_country,
  );
  drupal_add_js(array('webformElectedMep' => $js_settings), 'setting');
  drupal_add_js(drupal_get_path('module', 'webform_elected_mep') . '/js/webform_elected_mep.js');

  $element_name = 'submitted[' . $component['form_key'] . ']';
  drupal_add_js('Drupal.webformElectedMepSetDefaults("' . $element_name . '", ' . json_encode($country_default) . ', ' . json_encode($constituency_default) . ');', 'inline');
  // drupal_add_js('jQuery(document).ready(function () { Drupal.webformElectedMepSetDefaults("' . $element_name . '", ' . json_encode($country_default) . ', ' . json_encode($constituency_default) . '); });', 'inline');

  return $element;
}

/**
 * Implements _webform_submit_component().
 */
function _webform_submit_mep_constituency($component, $value) {
  return array($value);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_mep_constituency($component, $sids = array(), $single = FALSE, $join = NULL) {
  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('data'))
    ->condition('nid', $component['nid'])
    ->condition('cid', $component['cid']);

  if (count($sids)) {
    $query->condition('sid', $sids, 'IN');
  }

  if ($join) {
    $query->innerJoin($join, 'ws2_', 'wsd.sid = ws2_.sid');
  }

  $nonblanks = 0;
  $submissions = 0;

  // This could possibly be extended to count submissions by
  // country using the postal_code_validation API.
  $result = $query->execute();
  foreach ($result as $data) {
    if (drupal_strlen(trim($data['data'])) > 0) {
      $nonblanks++;
    }
    $submissions++;
  }

  $rows = array();
  $rows[0] = array(t('Left Blank'), ($submissions - $nonblanks));
  $rows[1] = array(t('User entered value'), $nonblanks);

  $header = array(t('Stat'), t('Value'));

  $other = array();
  $other[] = l(t('More information'), 'node/' . $component['nid'] . '/webform-results/analysis/' . $component['cid']);

  return array(
    'table_header' => $header,
    'table_rows' => $rows,
    'other_data' => $other,
  );
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_mep_constituency($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2][] = t('@name', array('@name' => $component['name']));
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_mep_constituency($component, $export_options, $value) {
  return _webform_elected_mep_constituency_values_to_string($component, $value);
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_mep_constituency($component, $value) {
  return _webform_elected_mep_constituency_values_to_string($component, $value);
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_mep_constituency() {
  return array(
    'webform_display_mep_constituency' => array(
      'render element' => 'element',
    ),
  );
}

/*
 * We do not need to define:
 * - _webform_attachments_component
 * - _webform_delete_component
 * - _webform_help_component
 */


/**
 * Creates string from array of MEP constituency values.
 * TODO Use component config for formatting.
 */
function _webform_elected_mep_constituency_values_to_string($component, $values) {
  $display_values = array();
  foreach ($values as $value) {
    list($country, $constituency) = explode('--', $value, 2);
    $display_value = ($country == $constituency) ? $country : $country . ' - ' . $constituency;
    $display_values[] = $display_value;
  }
  return check_plain(implode(', ', $display_values));
}
