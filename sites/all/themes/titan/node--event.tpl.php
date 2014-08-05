<?php switch($view_mode) { ?>
<?php case 'teaser': /****************************** teaser */ ?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix<?php if ($display_submitted): ?> indent<?php endif; ?>" <?php print $attributes; ?>>

  <div class="node-header">

    <?php if ($display_submitted): ?>
      <div class="date">

<?php

  // Some AK CSC changes here, to use the begin date for workshops instead of
  // the node creation date, which we use for other content types.

  if(!empty($node->field_date[$node->language][0]['value'])) {
    $raw_date = $node->field_date[$node->language][0]['value'];
    $date = explode(' ', $raw_date);
    $date_fields = explode('-', $date[0]);
    $time = mktime(0, 0, 0, $date_fields[1], $date_fields[2], $date_fields[0]);
  } else {
    $time = $node->created;
  }

?>
        <span class="month"><?php print format_date($time, 'custom', 'M') ?></span>
        <span class="day"><?php print format_date($time, 'custom', 'j') ?> </span>
        <span class="year"><?php print format_date($time, 'custom', 'Y') ?></span>
      </div>
    <?php endif; ?>

    <div class="title-and-meta">
      <?php print render($title_prefix); ?>
      <h2 class="node-title" <?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
      <?php
        print render($content['field_location']);
        print render($content['field_date']);
      ?>
      <?php print render($title_suffix); ?>

      <?php if ($display_submitted): ?>
        <div class="meta">
          <?php if ($display_submitted): ?>
            <span class="submitted">
              <?php print t('By ') . $name; ?>
            </span>
          <?php endif; ?>
        </div>
      <?php endif; ?>
    </div> <!-- /.title-and-meta -->
  </div> <!-- /.node-header -->

  <div class="content"<?php print $content_attributes; ?>>
    <?php //print $user_picture; ?>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      hide($content['field_location']);
      hide($content['field_date']);

      $block = module_invoke('node_subpages', 'block_view', 'menu_bar');
      print render($block['content']);

      print render($content);
    ?>

    <div class="links-wrapper">
      <?php print render($content['links']); ?>
    </div>
  </div>

  <?php print render($content['comments']); ?>

</div>
<?php break; default: /************************ all other display modes */ ?>
<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix<?php if ($display_submitted): ?> indent<?php endif; ?>" <?php print $attributes; ?>>

  <div class="node-header">

    <?php if ($display_submitted): ?>
      <div class="date">

<?php

  // Some AK CSC changes here, to use the begin date for workshops instead of
  // the node creation date, which we use for other content types.

  if($node->type == 'event' &&
     !empty($node->field_date[$node->language][0]['value'])) {
    $raw_date = $node->field_date[$node->language][0]['value'];
    $date = explode(' ', $raw_date);
    $date_fields = explode('-', $date[0]);
    $time = mktime(0, 0, 0, $date_fields[1], $date_fields[2], $date_fields[0]);
  } else {
    $time = $node->created;
  }

?>
        <span class="month"><?php print format_date($time, 'custom', 'M') ?></span>
        <span class="day"><?php print format_date($time, 'custom', 'j') ?> </span>
        <span class="year"><?php print format_date($time, 'custom', 'Y') ?></span>
      </div>
    <?php endif; ?>

    <div class="title-and-meta">
      <?php print render($title_prefix); ?>
	<?php // Some AK CSC changes here. Unlink page titles. ?>
      <h1 class="node-title" <?php print $title_attributes; ?>><?php print $title; ?></h1>
      <?php print render($title_suffix); ?>

      <?php if ($display_submitted): ?>
        <div class="meta">
          <?php if ($display_submitted): ?>
            <span class="submitted">
              <?php print t('By ') . $name; ?>
            </span>
          <?php endif; ?>
        </div>
      <?php endif; ?>
    </div> <!-- /.title-and-meta -->
  </div> <!-- /.node-header -->

  <div class="content"<?php print $content_attributes; ?>>
    <?php //print $user_picture; ?>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      hide($content['field_location']);
      hide($content['field_date']);

      print render($content['field_location']);
      print render($content['field_date']);

    ?>

    <?php
      $block = module_invoke('node_subpages', 'block_view', 'menu_bar');
      if (!empty($block['content']['#markup'])):
    ?>
    <div id="event-sub-nav">
      <?php print render($block['content']); ?>
    </div>
    <?php endif; ?>

    <?php

      print render($content);
    ?>

    <div class="links-wrapper">
      <?php print render($content['links']); ?>
    </div>
  </div>

  <?php print render($content['comments']); ?>

</div>
<?php } ?>
