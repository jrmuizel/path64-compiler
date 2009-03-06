package Date;

use Time::Local();

sub new
{
  my $class = shift;

  my $self = {};
  bless $self, $class;

  return $self;
}

sub now
{
  my $class = shift;

  my $self = new $class;

  ($self->{seconde},
   $self->{minute},
   $self->{heure},
   $self->{jour},
   $self->{mois},
   $self->{annee},
   $self->{jsemaine},
   $self->{jannee}) = localtime(time);

  $self->{annee} += 1900;
  $self->{mois}++;

  return $self;
}

sub fromStr
{
  my $class = shift;
  my $str = shift;

  my $self = new $class;

  ($self->{jour},
   $self->{mois},
   $self->{annee},
   $self->{heure},
   $self->{minute},
   $self->{seconde}) = split(/[\s\/\:]/, $str);

  my @tab = localtime(Time::Local->timelocal($self->{minute},
					     $self->{heure},
					     $self->{jour},
					     $self->{mois},
					     $self->{annee}
					     )
		      );
  $self->{jsemaine} = $tab[6];
  $self->{jannee} = $tab[7];

  return $self;
}

sub formatShort
{
  my $self = shift;
  return sprintf("%02d/%02d/%02d", 
		 $self->{jour},
		 $self->{mois},
		 $self->{annee}
		 );
}

sub formatLong
{
  my $self = shift;
  return sprintf("%02d/%02d/%02d %02d:%02d:%02d", 
		 $self->{jour},
		 $self->{mois},
		 $self->{annee},
		 $self->{heure},
		 $self->{minute},
		 $self->{seconde}
		 );
}

sub compare
{
  my $self = shift;
  if (!ref($self))
  {
      $self = shift;
  }
  my $date2 = shift;

  my $res = ($self->{annee} <=> $date2->{annee});
  if (!$res)
  {
      $res = ($self->{mois} <=> $date2->{mois});
      if (!$res)
      {
	  $res = ($self->{jour} <=> $date2->{jour});
	  if (!$res)
	  {
	      $res = ($self->{heure} <=> $date2->{heure});
	      if (!$res)
	      {
		  $res = ($self->{minute} <=> $date2->{minute});
		  if (!$res)
		  {
		      $res = ($self->{seconde} <=> $date2->{seconde});
		  }
	      }
	  }
      }
  }
  return $res;
}

1;

