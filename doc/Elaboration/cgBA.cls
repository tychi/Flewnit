%*******************************************************************************
%  cgDA.cls
%
%  LaTeX-Dokumentenklasse fuer Studien-/Diplomarbeiten der AG Computergraphik.
%
%  (C) 2003-2007 AG Computergraphik <cg@uni-koblenz.de>
%           Universitaet Koblenz-Landau
%
%  2003-10-09  v1.0  M. Geimer
%  2005-12-22  v1.1  M. Biedermann (neues FB4-Layout fuer DA)
%  2006-01-17  v1.2  M. Biedermann (neues FB4-Layout fuer DA, Einzelarbeit)
%  2007-02-21  v1.3  M. Biedermann (neues Uni-Logo eingebaut)
%  2007-03-21  v1.4  M. Biedermann (Bugfix Schriftgroesse, Dank an Stefan Mueller)
%  2007-04-07  v1.5  M. Biedermann (Englisch-Unterstuetzung wieder hinzugefuegt, 
%									Dank an Florian Schulze und Pascal Sproedt)
%  2007-05-01  v1.6  M. Biedermann (Englisch-Unterstuetzung geaendert (siehe README),
%									kleine Bugfixes)
%  2007-05-15  v1.7  M. Biedermann (Schriftgroesse etc. von Bildunterschriften
%									korrigiert, Dank an Christian Rieder)
%	 2008-05-27  v1.8  M. Raspe (Umlautcodierung korrigiert, Dank an Andreas von Arb)
%*******************************************************************************


%--- Identification ------------------------------------------------------------

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{cgBA}[2009/10/26 v0.815
                   Dokumentenklasse fuer Bachelorarbeiten
                   der AG Computergraphik]


%--- Initial code --------------------------------------------------------------

\RequirePackage{ifthen}
\newboolean{@isFemale}
\newboolean{@isTimes}
\newboolean{@isPalatino}
\newboolean{@isExtern}


%--- Declaration of options ----------------------------------------------------

\DeclareOption{w}{\setboolean{@isFemale}{true}}
\DeclareOption{m}{\setboolean{@isFemale}{false}}
\DeclareOption{times}{\setboolean{@isTimes}{true}}
\DeclareOption{palatino}{\setboolean{@isPalatino}{true}}
\DeclareOption{twoside}{\PassOptionsToClass{twoside}{article}}
\DeclareOption{extern}{\setboolean{@isExtern}{true}}


%--- Execution of options ------------------------------------------------------

\ProcessOptions


%--- Package loading -----------------------------------------------------------

\LoadClass[a4paper,11pt,titlepage]{article}
\RequirePackage[ngerman]{babel}			% Unterstuetzung englische und neue deutsche Rechtschreibung

\RequirePackage[utf8]{inputenc} 			% Unterstützung von Umlauten und ... 
\RequirePackage[T1]{fontenc}      			% Umlaute korrekt codieren
\RequirePackage{graphicx}				% fuer Einbindung von Grafiken
\RequirePackage{tabularx}				% flexible Gestaltung von Tabellen
\RequirePackage[hang,small,bf]{caption}			% Bildunterschriften mit Optionen
\RequirePackage{amsfonts}						%
\RequirePackage{amssymb}						%	Mathe-Zeugs
\RequirePackage{amsmath}						%
\RequirePackage{setspace} 				%Zeilenabstand �ndern
        						%       \singlespacing %einfach
        						%       \onehalfspacing %1,5-fach
        						%       \doublespacing %doppelt
						  	    %       \linespread{1.25}
        						%       \baselineskip15pt

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%				% Default: Computer Modern (TeX-Standard)
\ifthenelse{\boolean{@isTimes}}{%
         \RequirePackage{times}					% Schriftart Times New Roman
      }{}
\ifthenelse{\boolean{@isPalatino}}{%
         \RequirePackage{palatino}			% Schriftart Book Antiqua/Palatino
      }{}      
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


\setlength{\emergencystretch}{3em}



%--- Code ----------------------------------------------------------------------

\def\@erstgutachter{}
\newcommand{\erstgutachter}[1]{\def\@erstgutachter{#1}}

\def\@erstgutachterInfo{}
\newcommand{\erstgutachterInfo}[1]{\def\@erstgutachterInfo{#1}}

\def\@zweitgutachter{}
\newcommand{\zweitgutachter}[1]{\def\@zweitgutachter{#1}}

\def\@zweitgutachterInfo{}
\newcommand{\zweitgutachterInfo}[1]{\def\@zweitgutachterInfo{#1}}

\def\@externLogo{}
\newcommand{\externLogo}[2]{\def\@externLogo{\includegraphics[width={#1}]{{#2}}}}

\def\@externName{}
\newcommand{\externName}[1]{\def\@externName{#1}}


\newcommand{\abgabe}{%
   \ifcase\month\or
      Januar\or Februar\or M{\"a}rz\or April\or Mai\or Juni\or
      Juli\or August\or September\or Oktober\or November\or Dezember
   \fi\ \number\year
}


%%% save original offsets
\newlength{\voffsetBackup}
\setlength{\voffsetBackup}{\voffset}

\newlength{\hoffsetBackup}
\setlength{\hoffsetBackup}{\hoffset}

%%% preserve backslash (conflict with using \raggedleft
\newcommand{\PBS}[1]{\let\temp=\\#1\let\\=\temp}


%%% new titlepage
\renewcommand{\maketitle}{

   %%%  Titelseite  %%%
   \setlength{\voffset}{-.5in}
   \setlength{\hoffset}{-.25in}
   \begin{titlepage}
      \ifthenelse{\boolean{@twoside}}{%
         \setcounter{page}{0}
      }{%
         \setcounter{page}{-1}
      }
      % Logo Uni
      \hspace*{-1cm}
      % mit externem Logo
      \ifthenelse{\boolean{@isExtern}}{%
      	\begin{tabular*}{\linewidth}{p{.9cm}p{5cm}@{\hspace{2cm}}>{\PBS\raggedleft}p{6cm}}
 	     		{\includegraphics[height=.9cm]{logos/UniLogoNeu}}%
 	     	&%	
      		%\includegraphics[height=1.2cm]{logos/UniLogoNeu}%
      	&%
     			\@externLogo%
	      \\[-.1ex] %
	      &% 
     			\footnotesize \hspace{.1em}Fachbereich 4: Informatik%
      	
      	&%
      		\footnotesize \@externName%
      \end{tabular*}
      }{%
      % nur Uni-Logo
      	\begin{tabular*}{\linewidth}{p{1.5cm}p{7cm}}
 	     		\raisebox{-.2cm}[1.3cm][1.3cm]{\includegraphics[height=1.2cm]{logos/UniLogoNeu}}%
 	     	&%	
      		%\includegraphics[height=1.3cm]{logos/UniLogoNeu}%
	      \\[-1cm] %
	      &% 
     		\footnotesize \hspace{-.25em}Fachbereich 4: Informatik%
        \end{tabular*}
      }      
      \normalsize
      \vspace*{1.5cm}      
      \begin{center}
      \doublespacing
      {\huge\bfseries\@title}\\[1.25cm]
      \singlespacing 
     	\huge Bachelorarbeit\\[.2cm]
      \normalsize zur Erlangung des Grades eine%
      \ifthenelse{\boolean{@isFemale}}{%
      	r Bachelor of Science (B.Sc.)\\
      }{%
      	s Bachelor of Science (B.Sc.)\\
      }
      im Studiengang Computervisualistik\\[.75cm]
      {\large vorgelegt von\\[1ex]\Large\@author}\\[4cm]
			\end{center}	
			\normalsize	
      {\begin{tabular}{ll}
  	     Erstgutachter:   & \ifthenelse{\equal{\@erstgutachter}{}}{%
            {Prof. Dr.-Ing. Stefan M{\"u}ller}
            }{\@erstgutachter} \\
  	     { } & \ifthenelse{\equal{\@erstgutachterInfo}{}}{%
  	     		{(Institut f{\"u}r Computervisualistik, AG Computergraphik)}
  	     		}{\@erstgutachterInfo} \\[.2cm]
    	   Zweitgutachter: & \@zweitgutachter \\
    	   { } & \@zweitgutachterInfo \\
       \end{tabular}\\[2cm]}
  		\normalsize
  		\hspace*{.11cm}
      Koblenz, im \abgabe 
   \end{titlepage}

   % Rueckseite der Titelseite
   \ifthenelse{\boolean{@twoside}}{%
      \begin{titlepage}
         \setcounter{page}{0}
         \mbox{}   % Hier muss was stehen, also eine leere Box :-)
      \end{titlepage}
   }{}
   
   \setlength{\voffset}{\voffsetBackup}
   \setlength{\hoffset}{\hoffsetBackup}
   
   
	%%%  Erklaerung  %%%
	\noindent
	\Large{Erkl{\"a}rung}\\
	
	\vspace*{2ex}
	\noindent
	\normalsize
	Ich versichere, dass ich die vorliegende Arbeit selbst{\"a}ndig verfasst und keine anderen als die angegebenen Quellen und Hilfsmittel benutzt habe.\\
	
	\vspace*{2ex}
	
	\begin{flushleft}
	\begin{tabularx}{\linewidth}{lcc}
	{} & Ja & Nein \\
	{} & {} & {} \\
	Mit der Einstellung der Arbeit in die Bibliothek bin ich einverstanden. & $\square$ & $\square$ \\
	{} & {} & {} \\	
	Der Ver{\"o}ffentlichung dieser Arbeit im Internet stimme ich zu. & $\square$ & $\square$ 
	\end{tabularx}
	\end{flushleft}
	\vspace*{3cm}
	
	\small
	\noindent
	\dotfill \\
	{\small(Ort, Datum)}\hfill{\small (Unterschrift)}

	\normalsize   
	
}


