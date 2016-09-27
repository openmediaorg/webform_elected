<?php

// not used for now.

function webform_elected_mep_json_load($file_name) {
  $meps = array();
  $file_contents = file_get_contents($file_name);
  $data = json_decode($file_contents, TRUE);
  foreach ($data['meps'] as $raw_mep) {
    $mep = array(
      'unique_id' => isset($raw_mep['UserID']) ? $raw_mep['UserID'] : NULL,
      'jurisdiction' => webform_elected_mep_json_get_country($raw_mep),
      'first_name' => isset($raw_mep['Name']['sur']) ? $raw_mep['Name']['sur'] : NULL,
      'last_name' => isset($raw_mep['Name']['family']) ? $raw_mep['Name']['family'] : NULL,
      'full_name' => isset($raw_mep['Name']['full']) ? $raw_mep['Name']['full'] : NULL,
      // 'position' => 'Mayor',
      'email' => isset($raw_mep['Mail']['0']) ? $raw_mep['Mail']['0'] : NULL,
      'phone' => webform_elected_mep_json_get_phone($raw_mep),
      'twitter' => isset($raw_mep['Twitter']['0']) ? $raw_mep['Twitter']['0'] : NULL,
    );

    $meps[] = array_filter($mep);
  }
  return $meps;
}

function webform_elected_mep_json_get_country(array $raw_mep) {
  $time_spans = array();
  if (isset($raw_mep['Constituencies']) && is_array($raw_mep['Constituencies'])) {
    foreach ($raw_mep['Constituencies'] as $time_span) {
      if (isset($time_span['end'], $time_span['country'])) {
        $time_spans[$time_span['end']] = $time_span['country'];
      }
    }
  }
  ksort($time_spans);
  return array_pop($time_spans);
}

function webform_elected_mep_json_get_phone(array $raw_mep) {
  $places = variable_get('webform_elected_mep_phone_order', array('Brussels', 'Strasbourg'));
  foreach ($places as $place) {
    if (isset($raw_mep['Addresses'][$places]['Phone'])) {
      return $raw_mep['Addresses'][$places]['Phone'];
    }
  }
  return NULL;
}
