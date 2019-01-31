/**
 * @file
 * Provides an alternative attach behaviour and utility functions used in admin
 * for real-time previewing of an option set.
 */

(function ($) {
  Drupal.behaviors.beforeafterPreview = {
    attach: function (context, settings) {
      if (settings.beforeafterPreview) {
        var before_after_settings = settings.beforeafterPreview;
        $('#' + before_after_settings.container).once('beforeafter-preview', function() {

          var instance = Drupal.beforeAfter.instance.apply(this, [null, before_after_settings, 0, this]);

          Drupal.beforeAfterPreview.instance = instance;
          Drupal.beforeAfterPreview.listenToForm();
        });
      }
    }
  };

  Drupal.beforeAfterPreview = {
    instance: null
  };

  Drupal.beforeAfterPreview.listenToForm = function() {
    var $form = $('form#ctools-export-ui-edit-item-form');
    if (!$form.length) throw 'Unable to find form element';

    $('.vertical-tabs-panes input, .vertical-tabs-panes select', $form).change(Drupal.beforeAfterPreview.updateOptions.bind(this));

    $('.beforeafter-preview--reset', $form).click(function(e) {
      e.preventDefault();
      Drupal.beforeAfterPreview.updateOptions(true);
    });
  };

  Drupal.beforeAfterPreview.updateOptions = function(reset) {
    if (!this.instance) throw 'No beforeafter instance present';

    this.instance.settings = Drupal.beforeAfterPreview.getOptions();
    this.instance.addLinks();
    if (reset === true) {
      var _this = this;
      this.instance.$el
        .animate({opacity: 0}, 600, function() {
        _this.instance.reset();
        _this.instance.draw();
      })
        .animate({opacity: 1}, 600, function() {
        _this.instance.intro();
      });
    }
  };

  Drupal.beforeAfterPreview.getOptions = function() {
    var $form = $('form#ctools-export-ui-edit-item-form');
    if (!$form.length) throw 'Unable to find form element';

    var options = {};
    $.each($form.serializeArray(), function() {
      options[this.name] = this.value;
    });

    return Drupal.beforeAfterPreview.typecastOptions(options);
  };

  Drupal.beforeAfterPreview.typecastOptions = function(options) {
    for (var key in options) {
      if (!options.hasOwnProperty(key)) continue;
      switch (key) {
        // Strings
        case 'theme':
        case 'intro_easing':
        case 'before_link_text':
        case 'after_link_text':
        case 'link_anim_easing':
        case 'click_anim_easing':
        case 'flexslider_selector':
        case 'flexslider_anim_callback':
        case 'flexslider_anim_easing':

          break;

        // Integers
        case 'flexslider_anim_delay':
        case 'flexslider_anim_duration':
        case 'click_anim_duration':
        case 'click_anim_delay':
        case 'link_anim_duration':
        case 'link_anim_delay':
        case 'intro_delay':
        case 'intro_duration':
          options[key] = parseInt(options[key]);
          break;

        // Floats
        case 'intro_position':
        case 'init_position':
          options[key] = parseFloat(options[key]);
          break;

        // Booleans
        case 'flexslider_enable':
        case 'show_links':
        case 'intro_animate':
          options[key] = !!(parseInt(options[key]));
          break;

        default:
          delete options[key];
          break;
      }
    }

    return options;
  };
})(jQuery);