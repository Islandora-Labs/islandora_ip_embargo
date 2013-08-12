<?php
/**
 * @file
 * Holds rule hooks for islandroa_ip_embargo.
 */

/**
 * Implements hook_rules_event_info().
 */
function islandora_ip_embargo_rules_event_info() {
  return array(
    'islandora_ip_embargo_embargo_to_be_lifted' => array(
      'group' => t('Islandora IP Embargo'),
      'label' => t('Embargo to be lifted'),
      'variables' => array(
        'embargoed_object' => array(
          'type' => 'islandora_object',
          'label' => t('The embargoed object.'),
          'description' => t('A Tuque object, as an entity.'),
        ),
      ),
    ),
  );
}
