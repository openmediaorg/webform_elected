<?php

/**
 * Implements hook_webform_elected_get_data_source_type_info().
 */
function webform_elected_example_data_source_webform_elected_get_data_source_type_info() {
  return array(
    'webform_elected_example_data_source_1' => array(
      'title' => 'Example Data Source 1',
      'key_component_types' => array(
        'postal_code',
        // 'number',
        'textfield',
      ),
    ),
    'webform_elected_example_data_source_2' => array(
      'title' => 'Example Data Source 2',
      'key_component_types' => array(
        'postal_code',
        'number',
        // 'textfield',
      ),
    ),
  );
}

function webform_elected_example_data_source_is_our_type($data_source_type) {
  $our_data_source_types = webform_elected_example_data_source_webform_elected_get_data_source_type_info();
  return in_array($data_source_type, array_keys($our_data_source_types));
}

/**
 * Implements hook_webform_elected_data_source_settings_form_alter().
 */
function webform_elected_example_data_source_webform_elected_data_source_settings_form_alter(&$form, &$form_state, & $extra_alter_parameters) {
  $node = & $extra_alter_parameters['node'];
  $component = & $extra_alter_parameters['component'];
  $data_source_type = & $extra_alter_parameters['data_source_type'];
  $data_source_type_info = & $extra_alter_parameters['data_source_type_info'];

  if (webform_elected_example_data_source_is_our_type($data_source_type)) {
    $form['elected']['data_source']['test_data_source_settings'] = array(
      '#title'         => t('Test setting'),
      '#type'          => 'select',
      '#options'       => array(
        'setting01' => 'Test setting option 1 for ' . $data_source_type_info['title'],
        'setting02' => 'Test setting option 2 for ' . $data_source_type_info['title'],
      ),
      '#default_value' => $component['extra']['data_source']['test_data_source_settings'],
      '#description'   => t('Test UI for Data Source specific setting.'),
      '#required'      => TRUE,
      '#parents' => array('extra', 'data_source', 'test_data_source_settings'),
      '#weight' => 2,
    );
  }
}

/**
 * Implements hook_webform_elected_lookup().
 */
function webform_elected_example_data_source_webform_elected_lookup($key_component_value, $component) {
  $data_source_type = isset($component['extra']['data_source_type']) ? $component['extra']['data_source_type'] : '';
  if (webform_elected_example_data_source_is_our_type($data_source_type)) {
    $officials = array(
      array(
        'jurisdiction' => 'Vancouver',
        'first_name' => 'Example',
        'last_name' => 'Official',
        'full_name' => 'Dr. Example Official',
        'position' => 'Mayor',
        'email' => 'example01@example.com',
        'phone' => '604-123-4567',
        'twitter' => '@example_official_01',
      ),
      array(
        'jurisdiction' => 'Vancouver',
        'first_name' => 'Other Example',
        'last_name' => 'Official',
        'full_name' => 'Dr. Other Example Official',
        'position' => 'Counsellor',
        'email' => 'example02@example.com',
        'phone' => '604-234-5678',
        'twitter' => '@example_official_02',
      ),
    );
    return $officials;
  }
}

// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
// 
// /**
//  * Implements hook_().
//  */
// function webform_elected_example_data_source_() {
// }
