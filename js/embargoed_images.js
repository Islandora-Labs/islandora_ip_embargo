// Make embargoed thumbnails obvious.
jQuery(document).ready(function($) {
  $('.islandora_ip_embargo_embargoed').wrap('<div style="position:relative;"></div>');
  $('.islandora_ip_embargo_embargoed').after('<span style="position:absolute;' +
    'left:0em;text-decoration:none;color:red;font-size:2em">EMBARGOED</span>');
});
