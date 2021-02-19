#!/usr/bin/perl
# This makes an image or animation gallery page.
# Written by Christopher Thomas.

#
# Includes

use strict;



#
# Documentation

my ($helpscreen);
$helpscreen = << 'Endofblock';

This makes an image gallery or animation gallery web page.
Written by Christopher Thomas.

Usage:  makegallery  (options) (file and directory list)

Options:

--help              Displays this screen.
--outfile=(name)    Sets the output filename. Default "gallery.html".
--title=(title)     Sets the gallery web page title text.
--exclude=(regex)   Excludes all filenames/directories matching (regex).
--nostills          Suppress non-animated images.
--noanims           Suppress animated images.
--nolinks           Suppress hyperlinking images.
--nofolders         Suppress printing folder names.
--nonames           Suppress printing filenames.
--width=(spec)      Use <img width="(spec)"> to resize images.
--height=(spec)     Use <img height="(spec)"> to resize images.

Anything starting with "--" is assumed to be an option. Anything else is
assumed to be a file or directory to process.

Endofblock



#
# Constants

my (%config_defaults);

%config_defaults =
(
  'outfile' => 'gallery.html',
  'title' => 'Image Gallery',
  # Lists will end up copied by reference, but that's ok.
  'exclude_list' => [],
  'target_list' => []
);



#
# Functions


# Processes command-line arguments, returning a configuration hash.
# No arguments.
# Returns a pointer to a config hash, or undef on error.

sub ProcessArgs
{
  my ($thisarg, $config_p);
  my ($thislist_p);

  $config_p = {};

  # Copy defaults by value.
  %$config_p = %config_defaults;

  # Iterate through command-line parameters.
  foreach $thisarg (@ARGV)
  {
    if ($thisarg =~ m/^--(\w+)=(.*\S)/)
    {
      # This is a key/value pair.

      # Special-case lists.
      if ('exclude' eq $1)
      {
        $thislist_p = $$config_p{'exclude_list'};
        push @$thislist_p, $2;
      }
      else
      {
        # Normal key/value pair.
        $$config_p{$1} = $2;
      }
    }
    elsif ($thisarg =~ m/^--(\w+)/)
    {
      # This is a flag.
      $$config_p{$1} = 1;
    }
    elsif ($thisarg =~ m/^\s*(.*\S)/)
    {
      # Assume this is a target.
      $thislist_p = $$config_p{'target_list'};
      push @$thislist_p, $1;
    }
  }

  if (!(defined $ARGV[0]))
  { $$config_p{'help'} = 1; }

  return $config_p;
}



# Identifies the type of file we're dealing with.
# This is either 'anim', 'image', or 'other'.
# On error, 'error' or 'bogus' may be returned.
# Arg 0 is the full name of the file to check.
# Returns an identification string.

sub GetFileType
{
  my ($fpath, $ftype);
  my ($cmd, @result);

  $fpath = $_[0];
  $ftype = 'bogus';

  if (!(defined $fpath))
  {
    print "### [GetFileType]  Bad arguments.\n";
  }
  else
  {
    $cmd = "identify $fpath" . ' 2>&1';
    @result = `$cmd`;

    $ftype = 'error';

    if (defined $result[0])
    {
      $ftype = 'image';

      if ($result[0] =~ m/no decode delegate/)
      {
        $ftype = 'other';
      }
      if (1 < scalar(@result))
      {
        $ftype = 'anim';
      }
    }
  }

  return $ftype;
}



# Regex-based filter.
# This tests all substrings that follow a '/' against a regex list.
# Arg 0 is the full filename to check.
# Arg 1 points to a list of regexes to test.
# Returns 1 if the name (or portion following '/') matches and 0 if not.

sub DoesPathMatchRegex
{
  my ($fpath, $regexlist_p, $match);
  my ($thisregex, $thispath);

  $fpath = $_[0];
  $regexlist_p = $_[1];
  $match = 0;

  if (!( (defined $fpath) && (defined $regexlist_p) ))
  {
    print "### [DoesPathMatchRegex]  Bad arguments.\n";
  }
  else
  {
    foreach $thisregex (@$regexlist_p)
    {
      $thispath = $fpath;

      if ($thispath =~ m/$thisregex/)
      { $match = 1; }

      while ($thispath =~ m/^.*?\/(.*\S)/)
      {
        $thispath = $1;

        if ($thispath =~ m/$thisregex/)
        { $match = 1; }
      }
    }
  }

  return $match;
}



#
# Main Program

my ($config_p);
my ($oname);
my ($cmd);
my (@flist, @thisflist, $targlist_p, $thistarget);
my ($thisfile, $thisdir, $lastdir, $thisname);
my ($thistype, $want_images, $want_anims);
my ($want_links, $want_folders, $want_names);


$config_p = ProcessArgs();

if ( (!(defined $config_p)) || (defined $$config_p{'help'}) )
{
  print $helpscreen;
}
else
{
  $oname = $$config_p{'outfile'};
  if (!open(OUTFILE, ">$oname"))
  {
    print "### Unable to write to \"$oname\".\n";
  }
  else
  {
    # Page header.

    print OUTFILE << "Endofblock";
<html>
<head><title>$$config_p{'title'}</title></head>
<body>

<font size="+2">$$config_p{'title'}</font><br>
<br>
Endofblock


    # Build the target list.

    @flist = ();
    $targlist_p = $$config_p{'target_list'};
    foreach $thistarget (@$targlist_p)
    {
      $cmd = "find $thistarget|sort";
      @thisflist = `$cmd`;
      push @flist, @thisflist;
    }


    # Get convenience copies of various configuration flags.

    $want_images = 1;
    if (defined $$config_p{'nostills'})
    { $want_images = 0; }

    $want_anims = 1;
    if (defined $$config_p{'noanims'})
    { $want_anims = 0; }

    $want_links = 1;
    if (defined $$config_p{'nolinks'})
    { $want_links = 0; }

    $want_folders = 1;
    if (defined $$config_p{'nofolders'})
    { $want_folders = 0; }

    $want_names = 1;
    if (defined $$config_p{'nonames'})
    { $want_names = 0; }


    # Process each candidate in the target list.

    $lastdir = undef;
    foreach $thisfile (@flist)
    {
      chomp($thisfile);
# FIXME - Diagnostics.
#print STDERR "-- File: \"$thisfile\"\n";

      if (DoesPathMatchRegex($thisfile, $$config_p{'exclude_list'}))
      {
        # This is a manually-excluded file or path. Ignore it.
      }
      else
      {
        $thistype = GetFileType($thisfile);

        if ( (('image' eq $thistype) && $want_images)
          || (('anim' eq $thistype) && $want_anims) )
        {
          # This is a type of file we want to display.

          # Parse into path and filename.

          $thisdir = '(unknown)';
          $thisname = $thisfile;

          if ($thisfile =~ m/^(.*\/)(.*\S)/)
          {
            $thisdir = $1;
            $thisname = $2;
          }


          # Check folder name, and render if we have a new folder.
          if ( (!(defined $lastdir)) || ($thisdir ne $lastdir) )
          {
            $lastdir = $thisdir;
            if ($want_folders)
            {
              print OUTFILE << "Endofblock"

<br>
<br>
<b>$thisdir</b><br>
<br>

Endofblock
            }
          }


          # Write an entry for this image.

          if ($want_links)
          {
            print OUTFILE '<a href="' . $thisfile . '">';
          }

          print OUTFILE '<img src="' . $thisfile . '"';

          if (defined $$config_p{'width'})
          {
            print OUTFILE ' width="' . $$config_p{'width'} . '"';
          }

          if (defined $$config_p{'height'})
          {
            print OUTFILE ' height="' . $$config_p{'height'} . '"';
          }

          # FIXME - Put names in alt-tags, instead of as text.
          # Writing them as text would require using tables.
          if ($want_names)
          {
            # NOTE - Early browsers use "alt", later use "title", for
            # tooltip text. "Alt" is canonically what's shown when the
            # image is not displayed.
            print OUTFILE ' alt="' . $thisname . '"';
            print OUTFILE ' title="' . $thisname . '"';
          }

          print OUTFILE '</img>' . "\n";

          if ($want_links)
          {
            print OUTFILE '</a>' . "\n";
          }


          # Finished with this file.

        } # Is this a file type we want to process test.
      } # Exclusion list test.
    } # Filename iteration.


    # Page footer.

    print OUTFILE << "Endofblock";

</body>
</html>
Endofblock


    # Done.
    close(OUTFILE);
  }
}


#
# This is the end of the file.
