/**
 * @file
 * Show the overlay text for embargoed items.
 */
(function ($, Drupal, window, document, undefined) {
Drupal.behaviors.islandora_ip_embargo = {
  attach: function(context, settings) {
    jQuery(document).ready(function($) {
      $('.islandora_ip_embargo_embargoed').wrap('<div style="position:relative;"></div>');
      $('.islandora_ip_embargo_embargoed').after('<span class="islandora_ip_embargo_embargoed"></span>');
      $('.islandora_ip_embargo_embargoed').text(Drupal.t(Drupal.settings.islandora_ip_embargo.text));
      $('span.islandora_ip_embargo_embargoed').css({
        'color' : '#' + Drupal.settings.islandora_ip_embargo.color,
        'left' : '0',
        'padding' : '0.5em',
        'font-size' : '1.25em',
        'position' : 'absolute',
        'text-decoration' : 'none',
      });
    });
  }};
})(jQuery, Drupal, this, this.document);
