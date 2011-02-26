<?php
// $Id: page.tpl.php,v 1.18 2008/01/24 09:42:53 goba Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <title><?php print $head_title ?></title>
    <?php print $head ?>
    <?php print $styles ?>
    <?php print $scripts ?>
	 
    <!--[if IE]>
      <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path() . path_to_theme(); ?>/fix-ie.css" />
    <![endif]-->
	 <!--[if lt IE 7]>
      <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path() . path_to_theme(); ?>/fix-ie6.css" />
    <![endif]-->
	 
  <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1" />
  </head>
  <body id="homepage"<?php print phptemplate_body_class($left, $right); ?>>
	<!-- Layout -->
    <div id="wrapper">
    <div id="container" class="clear-block">
	 
	 	<div id="topbar" class="clearfix">
			<?php print $topbar; ?>
		</div>

      <div id="header" class="clearfix vcard">
			<div class="logo"></div>
			<h1 class="org"><?php print check_plain($site_name); ?></h1>
			<h2><?php print check_plain($site_slogan); ?></h2>
			<h3 class="tel">Call us<br /><em>020 7723 1216</em></h3>
			<div id="header-search">
		  		<?php print $header; ?>
			</div>
      </div> <!-- /header -->
		
		<div id="nav" class="clearfix">
			<?php
			  $my_menu = menu_tree('primary-links');
			  $my_menu = str_replace("leaf", "", $my_menu );
			  $my_menu = str_replace("active-trail", "active", $my_menu );
			  $my_menu = str_replace("menu", "", $my_menu );
			  $my_menu = str_replace("expanded", "", $my_menu );
			  $my_menu = str_replace(" class=\"\"", "", $my_menu );
			  print $my_menu;
			  ?>		
		</div>
		
		<div id="content" class="clearfix">
			
			<?php if ($banner): ?>
			<div id="banner" class="clearfix">
				<?php print $banner ?>
			</div>
			<?php endif; ?>
			
			<?php if ($left): ?>
			  <div id="sidebar-left" class="sidebar">
				 <?php print $left ?>
			  </div> <!-- /#sidebar-left -->
			<?php endif; ?>
	
			<div id="center">
				 <?php if ($tabs): print '<div id="tabs-wrapper" class="clear-block">'; endif; ?>
				 <?php if ($title): print '<h2'. ($tabs ? ' class="with-tabs"' : '') .'>'. $title .'</h2>'; endif; ?>
				 <?php if ($tabs): print '<ul class="tabs primary">'. $tabs .'</ul></div>'; endif; ?>
				 <?php if ($tabs2): print '<ul class="tabs secondary">'. $tabs2 .'</ul>'; endif; ?>
				 <?php if ($show_messages && $messages): print $messages; endif; ?>
				 <?php print $help; ?>
				 <div class="clear-block">
					<?php print $content ?>
				 </div>
				 <?php print $feed_icons ?>
				 
			</div> <!-- /#center -->
	
				
			<?php if ($right): ?>
			  <div id="sidebar-right" class="sidebar">
				 <?php print $right ?>
			  </div> <!-- /#sidebar-right -->
			<?php endif; ?>
			
			
		</div> <!-- /#content -->
		
		<div id="searchbar">
			<?php if ($search_box): ?><div class="block block-theme"><?php print $search_box ?></div><?php endif; ?>
		</div>
		
		<div id="footer" class="clearfix">
			<?php print $footer ?>
		</div>
		
    </div> <!-- /container -->
	 
	 <div id="container-foot"></div>
	 
	 <div id="credits">
	 	<?php print $credits ?>
	 </div>
	 
  </div> <!-- /wrapper -->
  <?php print $closure ?>
  </body>
</html>
