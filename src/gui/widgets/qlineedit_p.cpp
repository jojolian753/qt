/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlineedit.h"
#include "qlineedit_p.h"

#ifndef QT_NO_LINEEDIT

#include "qabstractitemview.h"
#include "qclipboard.h"
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif
#ifndef QT_NO_IM
#include "qinputcontext.h"
#include "qlist.h"
#endif

QT_BEGIN_NAMESPACE

const int QLineEditPrivate::verticalMargin(1);
const int QLineEditPrivate::horizontalMargin(2);

int QLineEditPrivate::xToPos(int x, QTextLine::CursorPosition betweenOrOn) const
{
    QRect cr = adjustedContentsRect();
    x-= cr.x() - hscroll + horizontalMargin;
    return control->xToPos(x, betweenOrOn);
}

QRect QLineEditPrivate::cursorRect() const
{
    QRect cr = adjustedContentsRect();
    int cix = cr.x() - hscroll + horizontalMargin;
    QRect crect = control->cursorRect();
    crect.moveTo(crect.topLeft() + QPoint(cix, vscroll));
    return crect;
}

#ifndef QT_NO_COMPLETER

void QLineEditPrivate::_q_completionHighlighted(QString newText)
{
    Q_Q(QLineEdit);
    if (control->completer()->completionMode() != QCompleter::InlineCompletion) {
        q->setText(newText);
    } else {
        int c = control->cursor();
        QString text = control->text();
        q->setText(text.left(c) + newText.mid(c));
        control->moveCursor(control->end(), false);
        control->moveCursor(c, true);
    }
}

#endif // QT_NO_COMPLETER

void QLineEditPrivate::_q_handleWindowActivate()
{
    Q_Q(QLineEdit);
    if (!q->hasFocus() && control->hasSelectedText())
        control->deselect();
}

void QLineEditPrivate::_q_textEdited(const QString &text)
{
    Q_Q(QLineEdit);
    emit q->textEdited(text);
#ifndef QT_NO_COMPLETER
    if (control->completer()
        && control->completer()->completionMode() != QCompleter::InlineCompletion)
        control->complete(-1); // update the popup on cut/paste/del
#endif
}

void QLineEditPrivate::_q_cursorPositionChanged(int from, int to)
{
    Q_Q(QLineEdit);
    q->update();
    emit q->cursorPositionChanged(from, to);
}

#ifdef QT_KEYPAD_NAVIGATION
void QLineEditPrivate::_q_editFocusChange(bool e)
{
    Q_Q(QLineEdit);
    q->setEditFocus(e);
}
#endif

void QLineEditPrivate::_q_selectionChanged()
{
    Q_Q(QLineEdit);
    if (!control->text().isEmpty() && control->preeditAreaText().isEmpty()) {
        QStyleOptionFrameV2 opt;
        q->initStyleOption(&opt);
        bool showCursor = control->hasSelectedText() ?
                          q->style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected, &opt, q):
                          q->hasFocus();
        setCursorVisible(showCursor);
    }

    emit q->selectionChanged();
}

void QLineEditPrivate::init(const QString& txt)
{
    Q_Q(QLineEdit);
    control = new QLineControl(txt);
    control->setFont(q->font());
    QObject::connect(control, SIGNAL(textChanged(QString)),
            q, SIGNAL(textChanged(QString)));
    QObject::connect(control, SIGNAL(textEdited(QString)),
            q, SLOT(_q_textEdited(QString)));
    QObject::connect(control, SIGNAL(cursorPositionChanged(int,int)),
            q, SLOT(_q_cursorPositionChanged(int,int)));
    QObject::connect(control, SIGNAL(selectionChanged()),
            q, SLOT(_q_selectionChanged()));
    QObject::connect(control, SIGNAL(accepted()),
            q, SIGNAL(returnPressed()));
    QObject::connect(control, SIGNAL(editingFinished()),
            q, SIGNAL(editingFinished()));
#ifdef QT_KEYPAD_NAVIGATION
    QObject::connect(control, SIGNAL(editFocusChange(bool)),
            q, SLOT(_q_editFocusChange(bool)));
#endif
    QObject::connect(control, SIGNAL(cursorPositionChanged(int,int)),
            q, SLOT(updateMicroFocus()));
    
    QObject::connect(control, SIGNAL(textChanged(const QString &)),
            q, SLOT(updateMicroFocus()));

    // for now, going completely overboard with updates.
    QObject::connect(control, SIGNAL(selectionChanged()),
            q, SLOT(update()));

    QObject::connect(control, SIGNAL(displayTextChanged(QString)),
            q, SLOT(update()));

    QObject::connect(control, SIGNAL(updateNeeded(QRect)),
            q, SLOT(update()));

    QStyleOptionFrameV2 opt;
    q->initStyleOption(&opt);
    control->setPasswordCharacter(q->style()->styleHint(QStyle::SH_LineEdit_PasswordCharacter, &opt, q));
#ifndef QT_NO_CURSOR
    q->setCursor(Qt::IBeamCursor);
#endif
    q->setFocusPolicy(Qt::StrongFocus);
    q->setAttribute(Qt::WA_InputMethodEnabled);
    //   Specifies that this widget can use more, but is able to survive on
    //   less, horizontal space; and is fixed vertically.
    q->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed, QSizePolicy::LineEdit));
    q->setBackgroundRole(QPalette::Base);
    q->setAttribute(Qt::WA_KeyCompression);
    q->setMouseTracking(true);
    q->setAcceptDrops(true);

    q->setAttribute(Qt::WA_MacShowFocusRect);
}

QRect QLineEditPrivate::adjustedContentsRect() const
{
    Q_Q(const QLineEdit);
    QStyleOptionFrameV2 opt;
    q->initStyleOption(&opt);
    QRect r = q->style()->subElementRect(QStyle::SE_LineEditContents, &opt, q);
    r.setX(r.x() + leftTextMargin);
    r.setY(r.y() + topTextMargin);
    r.setRight(r.right() - rightTextMargin);
    r.setBottom(r.bottom() - bottomTextMargin);
    return r;
}

void QLineEditPrivate::setCursorVisible(bool visible)
{
    Q_Q(QLineEdit);
    if ((bool)cursorVisible == visible)
        return;
    cursorVisible = visible;
    QRect r = cursorRect();
    if (control->inputMask().isEmpty())
        q->update(r);
    else
        q->update();
}

void QLineEditPrivate::updatePasswordEchoEditing(bool editing)
{
    Q_Q(QLineEdit);
    control->updatePasswordEchoEditing(editing);
    q->setAttribute(Qt::WA_InputMethodEnabled, shouldEnableInputMethod());
}

/*!
  This function is not intended as polymorphic usage. Just a shared code
  fragment that calls QInputContext::mouseHandler for this
  class.
*/
bool QLineEditPrivate::sendMouseEventToInputContext( QMouseEvent *e )
{
#if !defined QT_NO_IM
    Q_Q(QLineEdit);
    if ( control->composeMode() ) {
	int tmp_cursor = xToPos(e->pos().x());
	int mousePos = tmp_cursor - control->cursor();
	if ( mousePos < 0 || mousePos > control->preeditAreaText().length() ) {
            mousePos = -1;
	    // don't send move events outside the preedit area
            if ( e->type() == QEvent::MouseMove )
                return true;
        }

        QInputContext *qic = q->inputContext();
        if ( qic )
            // may be causing reset() in some input methods
            qic->mouseHandler(mousePos, e);
        if (!control->preeditAreaText().isEmpty())
            return true;
    }
#else
    Q_UNUSED(e);
#endif

    return false;
}

#ifndef QT_NO_DRAGANDDROP
void QLineEditPrivate::drag()
{
    Q_Q(QLineEdit);
    dndTimer.stop();
    QMimeData *data = new QMimeData;
    data->setText(control->selectedText());
    QDrag *drag = new QDrag(q);
    drag->setMimeData(data);
    Qt::DropAction action = drag->start();
    if (action == Qt::MoveAction && !control->isReadOnly() && drag->target() != q)
        control->removeSelection();
}

#endif // QT_NO_DRAGANDDROP

QT_END_NAMESPACE

#endif
