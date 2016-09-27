<?php

/**
 * @file
 *   Elected webform component.
 */

/**
 * Implements _webform_defaults_component().
 *
 * Specify the default properties of the component.
 *
 * @return
 *   An array defining the default structure of a component.
 */
function _webform_defaults_elected() {
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
      'data_source_type' => '',
      'data_source' => array(
        'key_component_cid' => '',
        'settings' => array(),
      ),

      'multiple' => 0,
      'autoselect_hide' => 0,

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
function _webform_display_elected($component, $value, $format = 'html') {
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'webform_display_elected',
    '#theme_wrappers' => array('html' == $format ? 'webform_element' : 'webform_element_text'),
    '#format' => $format,
    '#value' => _webform_elected_values_to_string($component, $value),
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
function _webform_edit_elected($component) {
  $form = array();

  $form['elected'] =  array(
    '#type' => 'fieldset',
    '#title' => t('Elected Configuration'),
    '#weight' => 1,
    '#collapsible' => TRUE,
  );

  $data_source_type_options = array('' => t('- Please choose a data source type -'));
  $data_source_type_infos = webform_elected_get_data_source_type_info();
  foreach ($data_source_type_infos as $type => $info) {
    $data_source_type_options[$type] = $info['title'];
  }

  $form['elected']['data_source_type'] = array(
    '#title'         => t('Data Source Type'),
    '#type'          => 'select',
    '#options'       => $data_source_type_options,
    '#default_value' => $component['extra']['data_source_type'],
    '#description'   => t('From what type of data source to find the elected officials'),
    '#required'      => TRUE,
    '#parents' => array('extra', 'data_source_type'),
    '#weight' => 0,
    '#ajax' => array(
      'callback' => '_webform_edit_elected_ajax_callback',
      'event' => 'change',
    ),
  );

  $form['elected']['data_source'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data Source Settings'),
    '#weight' => 1,
    '#collapsible' => TRUE,
    '#prefix' => '<div id="webform-elected-data-source-settings">',
    '#suffix' => '</div>',
  );

  // The data source settings are added in @see webform_elected_form_webform_component_edit_form_alter().

  $form['elected']['multiple'] = array(
    '#title'         => t('Allow Multiple Selections'),
    '#type'          => 'checkbox',
    '#default_value' => $component['extra']['multiple'],
    '#description'   => t('Enabling this option will allow multiple elected officials to be selected. Leaving this option unchecked will limit selection to only one elected official.'),
    '#parents' => array('extra', 'multiple'),
    '#weight' => 2,
  );

  $form['elected']['autoselect_hide'] = array(
    '#title'         => t('Auto-Select and Hide'),
    '#type'          => 'checkbox',
    '#default_value' => $component['extra']['autoselect_hide'],
    '#description'   => t('Enabling this option will automatically select the matching elected official(s) and hide the component from the user. If "Allow Multiple Selections" is enabled, all elected officials will be selected.  If "Allow Mutiple Selections" is disabled, the first matching elected official will be auto-selected.'),
    '#parents' => array('extra', 'autoselect_hide'),
    '#weight' => 3,
  );

  return $form;
}

/**
 *
 */
function _webform_render_elected($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $widget_type = 'checkboxes';
  if (! $component['extra']['multiple']) {
    $widget_type = 'radios';
  }
  $element = array(
    '#type' => $widget_type,
    '#options' => array(),
    '#title' => $filter ? _webform_filter_xss($component['name']) : $component['name'],
    '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
    '#required' => $component['mandatory'],
    '#weight' => $component['weight'],
    '#theme_wrappers' => array('webform_element'),
    '#pre_render' => array('webform_elected_pre_render_cleanup'),
  );
  if (isset($value) && $value) {
    $element['#options'] = $value;
    if ($component['extra']['multiple']) {
      $element['#default_value'] = $value;
    }
    else {
      $element['#default_value'] = reset($value);
    }
  }

  // Handle disabling.
  if (isset($component['extra']['disabled']) && $component['extra']['disabled']) {
    if ($filter) {
      $element['#attributes']['readonly'] = 'readonly';
    }
    else {
      $element['#disabled'] = TRUE;
    }
  }

  return $element;
}

/**
 * Implements _webform_submit_component().
 */
function _webform_submit_elected($component, $value) {
  // Otherwise we'd get the unchecked checkboxes as well, just with a "0" value.
  return array_filter($value);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_elected($component, $sids = array(), $single = FALSE, $join = NULL) {
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
function _webform_csv_headers_elected($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2][] = t('@name Email', array('@name' => $component['name']));
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_elected($component, $export_options, $value) {
  return _webform_elected_values_to_string($component, $value);
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_elected($component, $value) {
  return _webform_elected_values_to_string($component, $value);
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_elected() {
  return array(
    'webform_display_elected' => array(
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
 * Creates string from array of elected values.
 * TODO Use component config for formatting.
 */
function _webform_elected_values_to_string($component, $values) {
  $labels = array();
  foreach ($values as $full_unique_id) {
    $official = webform_elected_load_official($full_unique_id);
    $labels[] = webform_elected_get_option_label($component, $official);
  }
  return check_plain(implode(', ', $labels));
}
