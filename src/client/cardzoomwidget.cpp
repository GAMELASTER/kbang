/***************************************************************************
 *   Copyright (C) 2009 by MacJariel                                       *
 *   echo "badmailet@gbalt.dob" | tr "edibmlt" "ecrmjil"                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "cardzoomwidget.h"
#include "game.h"
#include "clienttypes.h"
#include <QTime>
#include <QTimer>
#include <QMouseEvent>

using namespace client;

int CardZoomWidget::sm_minZoomTime = 500;

/// @todo Review code after Game and MainWindow classes are cleaned up.

CardZoomWidget::CardZoomWidget(Game* game, CardWidget* cardWidget):
        CardWidget(game->mainWidget()),
        mp_game(game)
{
    clone(cardWidget);
    QPoint center = cardWidget->mapTo(mp_game->mainWidget(), cardWidget->center());
    setCardSizeRole(CARD_SIZE_ZOOMED);

    QPoint topLeft = center - QPoint((int)(qsize().width() / 2),
                                     (int)(qsize().height() / 2));
    QPoint bottomRight = topLeft + QPoint(qsize().width(), qsize().height());

    if (topLeft.x() < 0) {
        QPoint move = QPoint(-topLeft.x(), 0);
        topLeft += move;
        bottomRight += move;
    }
    if (topLeft.y() < 0) {
        QPoint move = QPoint(0, -topLeft.y());
        topLeft += move;
        bottomRight += move;
    }
    if (bottomRight.x() > game->mainWidget()->size().width()) {
        QPoint move = QPoint(game->mainWidget()->size().width() - bottomRight.x(), 0);
        topLeft += move;
        bottomRight += move;
    }
    if (bottomRight.y() > game->mainWidget()->size().height()) {
        QPoint move = QPoint(0, game->mainWidget()->size().height() - bottomRight.y());
        topLeft += move;
        bottomRight += move;
    }
    move(topLeft);
    validate();
    raise();
    show();
    grabMouse();
    mp_game->pauseGameEvents();
    m_time.start();
}


void CardZoomWidget::mouseReleaseEvent(QMouseEvent*)
{
    onRelease();
}

void CardZoomWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if (!rect().contains(ev->pos()))
        onRelease();
}

void CardZoomWidget::terminate()
{
    releaseMouse();
    mp_game->resumeGameEvents();
    deleteLater();
}

void CardZoomWidget::onRelease()
{
    int timeLeft = sm_minZoomTime - m_time.elapsed();
    if (timeLeft <= 0) {
        terminate();
    } else {
        QTimer::singleShot(timeLeft, this, SLOT(terminate()));
    }
}
