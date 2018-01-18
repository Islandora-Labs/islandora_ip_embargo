/**
 * @file
 * Show the overlay text for embargoed items.
 */

(function ($, Drupal) {
Drupal.behaviors.islandora_ip_embargo = {
  attach: function (context, settings) {
    $('img.islandora_ip_embargo_embargoed', context).once('islandora-ip-embargo-overlay', function () {
      $(this).wrap('<div style="position:relative;"></div>');
      $(this).after('<span class="islandora_ip_embargo_embargoed"></span>');
      $(this).nextAll('span:first')
        .text(Drupal.t(Drupal.settings.islandora_ip_embargo.text))
        .css({
          'color' : '#' + Drupal.settings.islandora_ip_embargo.color,
          'left' : '0',
          'padding' : '0.5em',
          'font-size' : '1.25em',
          'position' : 'absolute',
          'text-decoration' : 'none',
        });
    });
  }};
})(jQuery, Drupal);
