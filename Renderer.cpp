#include "Renderer.h"
#include <QtGui>

Renderer::Renderer(Simulation *sim,QSemaphore *sem)
{
	sema = sem;
	simulation = sim;
	colorMode = GENOME;
#ifdef SAVE_PICTURES_INTERVAL
	imageCounter = 0;
#endif
}
void Renderer::update(){
	updatePicture();
	QTimer::singleShot(UPDATE_INTERVAL, this, SLOT(update()));	
}

QColor Renderer::getColor(struct Cell *cell, int mode){
	int r = 0; //temporary red value
	int g = 0; //temporary green value
	int b = 0; //temporary blue value
	if(cell->place->wall && SHOW_LANDSCAPE){
	   r = 254;
	   g = 254;
	   b = 0;
	}else if(cell->place->dead && SHOW_LANDSCAPE){
	   r = 254;
	   g = 254;
	   b = 254;
	} else {
	switch(mode){
		case GENERATION:
		   r = qRed((cell->generation / 192) << 16);
		   g = qGreen((cell->generation << 3));
		   b = qBlue(cell->generation >> 2);
		   break;
		case GENOME:
				if(cell->generation >= LIVING_CELL){
					int hash = 0;
					for(uint i = 0; i < cell->size;i++ ){
						if(cell->genome[i] != cell->genome_operations - 1){
							hash += cell->genome[i];
                                                        hash %= 255;
						}else{
							break;
						}
					}
                                        r = hash*hash << 15;
                                        g = hash*123 << 2;
                                        b = hash;
				}
			break;
		case LINEAGE:
				if(cell->generation >= LIVING_CELL){
					r = qRed(cell->lineage * cell->lineage);
					g = qGreen(cell->lineage * cell->lineage);
					b = qBlue(cell->lineage * cell->lineage);
				}
			break;
		case HOME:
				if(cell->generation >= LIVING_CELL){
					r = cell->homePond;
					g = cell->homePond >> 5;
					b = cell->homePond >> 8;
				}
			break;
		case LANDSCAPE:{
				if(cell->generation >= LIVING_CELL){
					r = cell->size * 2 + 50;
					g = cell->size + 100;
					b = cell->size * 5;
				}
		}break;
		case ENERGY:
			r = qRed(cell->energy * 900);
			g = qGreen(cell->energy * 180);
			b = qBlue(cell->energy );
			break;
		case ENERGY2:
			r = qRed(cell->energy2 * 900);
			g = qGreen(cell->energy2 * 180);
			b = qBlue(cell->energy2 );
			break;
		case BAD:
			r = qRed(cell->bad * 900);
			g = qGreen(cell->bad * 180);
			b = qBlue(cell->bad * 50);
			break;
                case INJECTED:
                        if(cell->place->wall && SHOW_LANDSCAPE){
                                r = 254;
                                g = 254;
                                b = 0;
			}else if(cell->place->dead && SHOW_LANDSCAPE){
				r = 254;
				g = 254;
				b = 254;
                        }else{
                            if(cell->generation >= LIVING_CELL){
                                    r = qRed(cell->injected * cell->injected);
                                    g = qGreen(cell->injected * cell->injected);
                                    b = qBlue(cell->injected * cell->injected);
                            }
                        }
                        break;
                        break;
	}
	}
	return QColor(r % 255,g % 255,b % 255);
}

void Renderer::updatePicture(){	
	sema->acquire(1);
	int sizeX = simulation->x();
	int sizeY = simulation->y();
	
	QImage temp(sizeX,sizeY,QImage::Format_RGB32);
	simulation->pause();
	for(int x = 0; x < sizeX; x++){
		for(int y = 0; y < sizeY; y++){
			struct Cell *cell = simulation->cell(x,y,0);
			
			temp.setPixel(x,y,getColor(cell,colorMode).rgb());
		}
	}
	
	//int counter = simulation->counter();
	
	simulation->resume();
	setPixmap(QPixmap::fromImage (temp));
	//qDebug() << "Pond:"<< simulation->id() << "cells executed: " << counter;

#ifdef SAVE_PICTURES_INTERVAL
	if(imageCounter % SAVE_PICTURES_INTERVAL == 0) {
	   char buffer[80];
	   //qDebug()<<(QDir::homePath()+"/micro/asdf"+QString::number(imageCounter)+".png");
	   sprintf( buffer, "./micro/asdf%020d.png", imageCounter );
	   qDebug()<<buffer;
	   //temp.save(QDir::homePath()+"/micro/asdf"+QString::number(imageCounter)+".png");
	   temp.save(buffer);
	}
	imageCounter++;
	//qDebug()<<imageCounter;
#endif
	
	sema->release(1);
}

void Renderer::changeColorMode(int mode){
	colorMode = mode;
	updatePicture();
}

void Renderer::mousePressEvent ( QMouseEvent * event ){
	if(event->button() == Qt::RightButton){
		//changeColorMode((colorMode + 1) % RENDERMODES);		
	}else if(event->button() == Qt::LeftButton){
		simulation->pause();
		struct Cell *cell = simulation->cell(event->x(),event->y(),0);
		qDebug() << "try"<<event->x() << event->y();
		if(cell->generation >= LIVING_CELL){
			qDebug() << "got"<<event->x() << event->y();
			struct Cell tempCell = *cell;
			emit cellSelected(tempCell);
		}
		
		simulation->resume();
	}
}

Renderer::~Renderer()
{
}
