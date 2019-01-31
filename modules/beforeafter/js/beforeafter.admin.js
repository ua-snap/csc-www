/**
 * @file
 * Provides functions for the admin pages.
 */

(function ($) {
  Drupal.behaviors.beforeAfterAdmin = {
    attach: function (context) {

      // General
      $('fieldset#edit-general', context).drupalSetSummary(function(context) {
        var vals = [];

        vals.push(Drupal.t('@label: %value', {
          '@label': Drupal.t('Theme'),
          '%value': $('select[name="theme"] option:selected', context).text()
        }));
        if ($('input[name="show_links"]', context).is(':checked')) {
          vals.push(Drupal.t('Show full links'));
        }

        return vals.join('<br>');
      });

      // Intro
      $('fieldset#edit-intro', context).drupalSetSummary(function(context) {
        var vals = [];
        if ($('input[name="intro_animate"]', context).is(':checked')) {
          vals.push(Drupal.t('Animated'));

          $('input[name="intro_delay"], input[name="intro_duration"], select[name="intro_easing"]', context).each(function() {
            var label = $('label[for="' + $(this).attr('id') + '"]').text();
            vals.push(Drupal.t('@label: %value!suffix', {
              '@label': label,
              '%value': $(this).val(),
              '!suffix': (!isNaN(parseInt($(this).val())) ? 'ms' : '')
            }));
          });
        } else {
          vals.push(Drupal.t('Not animated'));
        }
        return vals.join('<br>');
      });

      // Flexslider
      $('fieldset#edit-flexslider', context).drupalSetSummary(function(context) {
        var vals = [];
        if ($('input[name="flexslider_enable"]', context).is(':checked')) {
          vals.push(Drupal.t('Enabled'));

          var anim_callback = $('select[name="flexslider_anim_callback"]', context).val();
          if (anim_callback) {
            vals.push(Drupal.t('Animate using %easing easing for %durationms on %callback, delayed by %delayms', {
              '%callback': anim_callback,
              '%easing': $('select[name="flexslider_anim_easing"]', context).val(),
              '%duration': $('input[name="flexslider_anim_duration"]', context).val(),
              '%delay': $('input[name="flexslider_anim_delay"]', context).val()
            }));
          }

        } else {
          vals.push(Drupal.t('Not enabled'));
        }

        return vals.join('<br>');
      });

    }
  };
})(jQuery);