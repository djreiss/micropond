#ifndef RENDERER_H_
#define RENDERER_H_

#include <QtGui>
#include "Simulation.h"

#define UPDATE_INTERVAL 200

#define GENERATION 0
#define GENOME 1
#define LINEAGE 2
#define HOME 3
#define LANDSCAPE 4
#define ENERGY 5
#define ENERGY2 6
#define BAD 7
#define INJECTED 8

#define RENDERMODES 9

#define LIVING_CELL 3

#define SHOW_LANDSCAPE true

// ffmpeg -r 10 -qscale 1 -i asdf%d.png micropond.avi
// this works for me (DJR):
// foreach joe (*.png) convert -geometry 50% $joe `echo $joe | sed 's/png/jpg/'`; end
// mencoder "mf://*.jpg" -o zzz.mpeg -ovc lavc -lavcopts vcodec=mpeg4
// or just this (convert not necessary; assumes original size is 1024x800):
// mencoder "mf://*.png" -o zzz.mpeg -vf scale=512:400 -ovc lavc -lavcopts vcodec=mpeg4
// this is good, then take the output of this to shrink to 1/2 size using HandBrake:
// mencoder "mf://micro/*.png" -o zzzz.mpg -of mpeg -mf w=1024:h=800:fps=25:type=png -ovc lavc -lavcopts vcodec=mpeg1video -oac copy
#define SAVE_PICTURES_INTERVAL 20 // every 20th screen refresh, save an image


class Renderer: public QLabel
{
	Q_OBJECT
public:
	Renderer(Simulation *sim,QSemaphore *sem);
	virtual ~Renderer();

	void mousePressEvent ( QMouseEvent * event );
	static QColor getColor(struct Cell *cell, int mode);
	void setSimulation(Simulation *sim){
		simulation->resume();
		simulation = sim;
	}

signals:
	void cellSelected(struct Cell cell);

public slots:
	void update();
	void changeColorMode(int mode);

private:
	void updatePicture();
	Simulation *simulation;
	int colorMode;
	QSemaphore *sema; //controls the rendering and loading

#ifdef SAVE_PICTURES_INTERVAL
	int imageCounter;
#endif
};

#endif /*RENDERER_H_*/
