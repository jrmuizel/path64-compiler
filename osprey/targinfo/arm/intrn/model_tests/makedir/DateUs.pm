package DateUs;

@ISA = qw(Date);

use File::Basename;

my $base_root=dirname($0);
require "$base_root/Date.pm";

@mois = ('Jan','Feb','Mar','Apr','May','Ju','Jul','Aug','Sep','Oct','Nov','Dec');
@jours = ('Sun','Mon','Tue','Wed','Thu','Fri','Sat');

sub new
{
  my $class = shift;

  my $self = new Date();
  bless $self, $class;

  return $self;
}

sub formatShort
{
  my $self = shift;
  return sprintf("%s %s %02d %02d", 
		 $jours[$self->{jsemaine}],
		 $mois[$self->{mois}-1],
		 $self->{jour},
		 $self->{annee}
		 );
}

sub formatLong
{
  my $self = shift;
  return sprintf("%s %s %02d %02d %02d:%02d:%02d", 
		 $jours[$self->{jsemaine}],
		 $mois[$self->{mois}-1],
		 $self->{jour},
		 $self->{annee},
		 $self->{heure},
		 $self->{minute},
		 $self->{seconde}
		 );
}

1;

