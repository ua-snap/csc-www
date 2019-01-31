/**
 * @file
 * Provides the widget and the Drupal behaviour which creates it.
 */

(function ($) {
  Drupal.behaviors.beforeafter = {
    attach: function (context, settings) {
      if (settings.beforeafter) {
        // Looping through all image fields with before/after formatting enabled on a page.
        $.each(settings.beforeafter, function (index, before_after_instances) {
          // Looping through each image pair of that field.
          $.each(before_after_instances, function (index, before_after_settings) {
            $('#' + before_after_settings.container).once('beforeafter', Drupal.beforeAfter.instance.bind(this, before_after_settings, this));
          })
        })
      }
    }
  };

  /**
   *
   * @param settings
   * @param delta
   * @param el
   *
   * @returns {Drupal.beforeAfter}
   */
  Drupal.beforeAfter = function(settings, delta, el) {
    this.el = el;
    this.$el = $(el);
    this.id = this.el.id;
    this.settings = settings;
    this.reset();
    this.init();
    this.addLinks();

    this.intro();

    if (settings.flexslider_enable) {
      this.$flexslider = this.$el.closest('.flexslider');
      if (!this.$flexslider.length) {
        this.$flexslider = null;
      }
      if (this.$flexslider) {
        var slider;
        var findFlexslider = function () {
          slider = this.$flexslider.data('flexslider');
          if (slider) {
            this.onFlexsliderInit(slider);
            return;
          }
          setTimeout(findFlexslider.bind(this), 100);
        }.bind(this);

        findFlexslider();
      }
    }

    return this;
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.init = function() {
    if (this.initialized) return;

    var settings = this.settings;

    this.images = $('img', this.el);

    this.images.wrap('<div class="beforeafter--image"></div>');
    this.$after = $(this.images[0]).parent();

    this.$slider = $('<input type="range" class="beforeafter--slider">').attr({
      value: this.state['pos'],
      max: this.state['max'],
      min: this.state['min']
    });
    this.$el.append(this.$slider);

    // It's currently impossible to get proper, cross-browser styling, so...
    this.$thumb = $('<span class="beforeafter--thumb"><i><b></b></i></span>');
    this.$el.append(this.$thumb);

    this.$el.wrapInner('<div class="beforeafter--wrap"></div>');

    var $links = $('<div class="beforeafter--links"></div>'),
        $linkBefore = $('<button class="beforeafter--link beforeafter--link-before"></button>').text(settings.before_link_text),
        $linkAfter = $('<button class="beforeafter--link beforeafter--link-after"></button>').text(settings.after_link_text);
    $links.append($linkBefore, $linkAfter);

    this.$links = $links;


    this.$el.addClass('beforeafter-' + settings.theme);

    this.activate();

    this.initialized = true;
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.addLinks = function() {
    if (this.settings.show_links) {
      this.$links.appendTo(this.$el);
    } else {
      this.$links = this.$links.detach();
    }
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.reset = function() {
    this.state = {
      min: 0,
      max: 1000,
      pos: 0,
      engaged: false
    };
    this.state['pos'] = (parseFloat(this.settings.init_position || 0) * this.state['max']);
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.intro = function() {
    var settings = this.settings,
        start_pos = (parseFloat(settings.intro_position || 0.5)*this.state['max']);

    if (settings.intro_animate) {
      this.draw();
      this.animate(
        start_pos,
        settings.intro_delay,
        settings.intro_duration,
        settings.intro_easing
      );
    } else {
      this.state['pos'] = start_pos;
      this.draw();
    }
  };

  /**
   *
   * @param pos
   * @param delay
   * @param duration
   * @param easing
   */
  Drupal.beforeAfter.prototype.animate = function(pos, delay, duration, easing) {
    var _this = this;
    this.animating = true;
    setTimeout(function() {
      $(_this.state).animate({
        pos: pos
      }, {
        duration: duration || 0,
        easing: easing || 'swing',
        step: function() {
          _this.draw();
        },
        complete: function() {
          _this.$slider.val(this.pos);
          _this.animating = false;
        }
      });
    }, delay || 0);
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.activate = function() {
    var _this = this;

    this.$slider.rangeslider({
      // Overload to trigger our own mouseUp and mouseDown handlers.
      onInit: function() {
        var Proto = this.constructor.prototype;
        this.handleDown = function(e) {
          if (_this.animating) return;
          _this.onMouseDown.apply(_this, arguments);
          Proto.handleDown.apply(this, arguments);
        }.bind(this);

        this.handleEnd = function(e) {
          _this.onMouseUp.apply(_this, arguments);
          Proto.handleEnd.apply(this, arguments);
        }.bind(this);
      }
    });

    this.$slider.on('input change', this.onSlide.bind(this));
    this.$slider.on('mousedown', this.onMouseDown.bind(this));
    this.$slider.on('mouseup', this.onMouseUp.bind(this));

    $('.beforeafter--link-before', this.$links).on('click', this.onLinkBefore.bind(this));
    $('.beforeafter--link-after', this.$links).on('click', this.onLinkAfter.bind(this));

    this.preventDraw = 5;
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.deactivate = function() {
    this.$slider.off('input change', this.onSlide.bind(this));
    this.$slider.off('mousedown', this.onMouseDown.bind(this));
    this.$slider.off('mouseup', this.onMouseUp.bind(this));

    $('.beforeafter--link-before', this.$links).off('click', this.onLinkBefore.bind(this));
    $('.beforeafter--link-after', this.$links).off('click', this.onLinkAfter.bind(this));
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onLinkBefore = function(e) {
    e.preventDefault();
    this.animate(
      this.state['min'],
      this.settings.link_anim_delay,
      this.settings.link_anim_duration,
      this.settings.link_anim_easing
    );
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onLinkAfter = function(e) {
    e.preventDefault();
    this.animate(
      this.state['max'],
      this.settings.link_anim_delay,
      this.settings.link_anim_duration,
      this.settings.link_anim_easing
    );
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onFlexsliderInit = function(slider) {
    if (!slider.vars.beforeAfterInstances) {
      slider.vars.beforeAfterInstances = {};
      slider.vars.before = Drupal.beforeAfter.onFlexsliderCallback.bind(Drupal.beforeAfter, 'before');
      slider.vars.after = Drupal.beforeAfter.onFlexsliderCallback.bind(Drupal.beforeAfter, 'after');
    }
    slider.vars.beforeAfterInstances[this.id] = this;
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onFlexsliderCallback = function(callback, slider) {
    var settings = this.settings;
    if (callback === settings.flexslider_anim_callback) {
      // Skip animation if the user has done anything
      if (this.state['engaged']) return;

      var start_pos = (parseFloat(settings.init_position)*this.state['max']),
          end_pos = (parseFloat(settings.intro_position)*this.state['max']);

      this.state['pos'] = start_pos;
      this.draw();
      this.animate(
        end_pos,
        settings.flexslider_anim_delay,
        settings.flexslider_anim_duration,
        settings.flexslider_anim_easing
      );
    }
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onMouseDown = function(e) {
    // Activate the onSlide event counter
    this.preventDraw = 0;

    this.state['speed'] = 0;

    // Flag to indicate user activity
    if (!this.state['engaged']) {
      this.state['engaged'] = true;
    }
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onMouseUp = function(e) {
    // Detect click (no significant dragging was done)
    if (this.preventDraw < 5) {
      this.animate(
        parseInt(this.$slider.val()),
        this.settings.click_anim_delay,
        this.settings.click_anim_duration,
        this.settings.click_anim_easing
      );
      return;
    }

    if (this.state['speed']) {
      var maxSpeed = 5;
      var deltaTime = Math.max((e.timeStamp - this.state['timestamp']), 1);
      var speed = Math.max(
        Math.min((this.state['speed'] / deltaTime), maxSpeed),
        -maxSpeed
      );

      var pos = parseInt(this.state['pos'] - (speed * 30));
      this.animate(pos, 0, 300, 'swing');
    }

    // Deactivate the onSlide event counter
    this.preventDraw = 5;
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.onSlide = function(e) {
    // Prevent draw for the first 5 events, to detect click behaviour
    if (this.preventDraw < 5) {
      // Animate to current value when exceeding the 5 events limit
      if (++this.preventDraw === 5) {
        this.animate(parseInt(this.$slider.val()), 0, 100);
      }
      return;
    }

    // Prevent rendering if we're animating
    if (this.animating) {
      return;
    }

    var pos = parseInt(this.$slider.val());
    this.state['timestamp'] = e.timeStamp;
    this.state['speed'] = this.state['pos']-pos;
    this.state['pos'] = pos;

    // @TODO: proper fallback for this
    if (requestAnimationFrame) {
      requestAnimationFrame(this.draw.bind(this));
    } else {
      this.draw();
    }
  };

  /**
   *
   */
  Drupal.beforeAfter.prototype.draw = function(e) {
    var percent = this.state['pos']/this.state['max'] * 100;
    this.$thumb.css('left', percent + '%');
    this.$after.css('width', percent + '%');
  };


  /**
   * Static methods
   */
  Drupal.beforeAfter.instance = function() {
    if (this instanceof Drupal.beforeAfter) {
      return this;
    } else {
      return new (Drupal.beforeAfter.bind.apply(Drupal.beforeAfter, arguments))();
    }
  };

  Drupal.beforeAfter.onFlexsliderCallback = function(callback, slider) {
    var currentIndex = slider.animatingTo || slider.currentSlide,
      slide = slider.slides[currentIndex],
      beforeAfterEl = $('.beforeafter', $(slide))[0],
      id = beforeAfterEl.id,
      beforeAfterInstance = slider.vars.beforeAfterInstances[id];
    if (!beforeAfterInstance) {
      return;
    }
    Drupal.beforeAfter.prototype.onFlexsliderCallback.apply(beforeAfterInstance, arguments);
  };

})(jQuery);