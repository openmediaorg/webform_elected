<?php

/**
 * Implements hook_webform_elected_get_data_source_type_info().
 */
function webform_elected_mep_webform_elected_get_data_source_type_info() {
  return array(
    'mep' => array(
      'title' => 'Members of European Parliament',
      'key_component_types' => array(
        'mep_constituency',
      ),
      'unique_only_by_component' => FALSE,
    ),
  );
}

function webform_elected_mep_is_our_type($data_source_type) {
  $our_data_source_types = webform_elected_mep_webform_elected_get_data_source_type_info();
  return in_array($data_source_type, array_keys($our_data_source_types));
}

/**
 * Implements hook_webform_elected_data_source_settings_form_alter().
 */
function webform_elected_mep_webform_elected_data_source_settings_form_alter(&$form, &$form_state, & $extra_alter_parameters) {
  $node = & $extra_alter_parameters['node'];
  $component = & $extra_alter_parameters['component'];
  $data_source_type = & $extra_alter_parameters['data_source_type'];
  $data_source_type_info = & $extra_alter_parameters['data_source_type_info'];

  if (webform_elected_mep_is_our_type($data_source_type)) {
    // $form['elected']['data_source']['some_mep_settings'] = array(
    //   '#title'         => t('Some setting specific to MEP'),
    //   '#type'          => 'select',
    //   '#options'       => array(
    //     'setting01' => 'MEP Test setting option 1 for ' . $data_source_type_info['title'],
    //     'setting02' => 'MEP Test setting option 2 for ' . $data_source_type_info['title'],
    //   ),
    //   '#default_value' => $component['extra']['data_source']['some_mep_settings'],
    //   '#description'   => t('Test UI for MEP Data Source specific setting.'),
    //   '#required'      => TRUE,
    //   '#parents' => array('extra', 'data_source', 'some_mep_settings'),
    //   '#weight' => 2,
    // );
  }
}

/**
 * Implements hook_webform_elected_lookup().
 */
function webform_elected_mep_webform_elected_lookup($key_component_value, $component) {
  // $key_component_value = is_array($key_component_value) ? array_pop($key_component_value) : $key_component_value;
  $data_source_type = isset($component['extra']['data_source_type']) ? $component['extra']['data_source_type'] : '';
  if (webform_elected_mep_is_our_type($data_source_type)) {
    list($country, $constituency) = explode('--', $key_component_value, 2);
    $officials = db_select('webform_elected_mep', 'mep')
      ->fields('mep')
      ->condition('mep.country', $country)
      ->condition('mep.constituency', $constituency)
      ->execute()
      ->fetchAllAssoc('unique_id', PDO::FETCH_ASSOC);
    return $officials;
  }
  return array();
}

// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_mep_() {
// }
