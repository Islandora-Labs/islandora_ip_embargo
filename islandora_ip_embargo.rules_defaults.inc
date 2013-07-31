<?php
/**
 * @file
 * Holds default rules for islandroa_ip_embargo.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function islandora_ip_embargo_default_rules_configuration() {
  $configs = array();

  $rule = rules_reaction_rule();

  $rule->label = 'E-mail admin';
  $rule->active = TRUE;
  $rule
    ->event('islandora_ip_embargo_embargo_to_be_lifted')
    ->action('mail', array(
      'to' => '[site:mail]',
      'subject' => '[site:name] "[embargoed_object:label]" will have its embargo lifted.',
      'message' => '[embargoed_object:label] with PID [embargoed_object:id] at ' .
        '[site:url]/islandora/object/[embargoed_object:id] is having its embargo lifted.',
    )
  );

  $configs['islandora_ip_embargo_embargo_to_be_lifted_notify_admin'] = $rule;

  return $configs;
}
