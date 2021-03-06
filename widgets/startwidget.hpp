/****************************************************************
 *
 * <Copyright information>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Author: Erik Türke, tuerke@cbs.mpg.de
 *
 * startwidget.hpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#ifndef STARTWIDGET_HPP
#define STARTWIDGET_HPP


#include "ui_start.h"
#include "qviewercore.hpp"

namespace isis
{
namespace viewer
{
namespace widget
{


class StartWidget : public QDialog
{
	Q_OBJECT
public:
	StartWidget( QWidget *parent, QViewerCore *core );

public Q_SLOTS:
	void openImageButtonClicked();
	void showMeChecked( bool );
	virtual void showEvent( QShowEvent * ) ;
	virtual void closeEvent( QCloseEvent * );
	virtual void keyPressEvent( QKeyEvent * );
	void openFavPath();
	void openRecentPath();

private:
	Ui::startDialog m_Interface;
	QViewerCore *m_ViewerCore;

	bool fillList( const _internal::FileInformationMap &fileInfoList, QListWidget *list );
};


}
}
}




#endif
