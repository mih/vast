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
 * imageStackWidget.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "imageStackWidget.hpp"
#include <viewercorebase.hpp>
#include <qviewercore.hpp>
#include <uicore.hpp>
#include <color.hpp>

namespace isis
{
namespace viewer
{
namespace ui
{

ImageStack::ImageStack( QWidget *parent, ImageStackWidget *widget )
	: QListWidget( parent ),
	  m_Widget( widget )
{
	setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setVerticalScrollMode( ScrollPerItem );
	setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
	setSortingEnabled( false );
}

void ImageStack::contextMenuEvent( QContextMenuEvent *event )
{

	QMenu menu( this );
	menu.addAction( m_Widget->m_Interface.actionClose_image );
	QMenu *imageTypeMenu = new QMenu( tr( "Image type" ), this );
	imageTypeMenu->addAction( m_Widget->m_Interface.actionStructural_image );
	imageTypeMenu->addAction( m_Widget->m_Interface.actionImage_type_stats );
	//TODO
	//  menu.addMenu( imageTypeMenu );
	menu.addSeparator();
	menu.addAction( m_Widget->m_Interface.actionDistribute_images );
	menu.addAction( m_Widget->m_Interface.actionClose_all_images );
	menu.exec( event->globalPos() );
}

void ImageStack::mousePressEvent( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton && geometry().contains( e->pos() ) && QApplication::keyboardModifiers() == Qt::ControlModifier ) {
		QDrag *drag = new QDrag( this );
		QMimeData *mimeData = new QMimeData;
		mimeData->setText( itemAt( e->pos() )->text() );
		drag->setMimeData( mimeData );
		drag->setPixmap( QIcon( ":/common/vast.jpg" ).pixmap( 15 ) );
		drag->exec();
	}

	QListWidget::mousePressEvent( e );
}



//ImageStackWidget


ImageStackWidget::ImageStackWidget( QWidget *parent, QViewerCore *core )
	: QWidget( parent ), m_ViewerCore( core )
{
	m_Interface.setupUi( this );
	m_Interface.actionClose_image->setIconVisibleInMenu( true );
	m_Interface.actionDistribute_images->setIconVisibleInMenu( true );
	m_Interface.actionClose_all_images->setIconVisibleInMenu( true );
	m_ImageStack = new ImageStack( this, this );
	m_Interface.stackLayout->addWidget( m_ImageStack );
	//  m_ImageStack->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );

	m_ImageStack->setEditTriggers( QAbstractItemView::NoEditTriggers );
	connect( m_ImageStack, SIGNAL( itemActivated( QListWidgetItem * ) ), this, SLOT( itemSelected( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemChanged( QListWidgetItem * ) ), this, SLOT( itemChanged( QListWidgetItem * ) ) );
	connect( m_ImageStack, SIGNAL( itemPressed( QListWidgetItem * ) ), this, SLOT( itemClicked( QListWidgetItem * ) ) );
	connect( m_Interface.actionClose_image, SIGNAL( triggered() ), this, SLOT( closeImage() ) );
	connect( m_Interface.actionDistribute_images, SIGNAL( triggered() ), this, SLOT( distributeImages() ) );
	connect( m_Interface.actionClose_all_images, SIGNAL( triggered() ), this, SLOT( closeAllImages() ) );
	connect( m_Interface.checkViewAllImages, SIGNAL( clicked( bool ) ), this, SLOT( viewAllImagesClicked() ) );
	connect( m_Interface.moveDown, SIGNAL( clicked( bool ) ), this, SLOT( moveDown() ) );
	connect( m_Interface.moveUp, SIGNAL( clicked( bool ) ), this, SLOT( moveUp() ) );

}

void ImageStackWidget::viewAllImagesClicked()
{
	m_ViewerCore->getSettings()->setPropertyAs<bool>( "viewAllImagesInStack", m_Interface.checkViewAllImages->isChecked() );
	synchronize();
}



void ImageStackWidget::synchronize()
{
	setVisible( m_ViewerCore->hasImage() );
	m_Interface.frame->setMaximumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "maxOptionWidgetHeight" ) );
	m_Interface.frame->setMinimumHeight( m_ViewerCore->getSettings()->getPropertyAs<uint16_t>( "minOptionWidgetHeight" ) );

	disconnect( m_Interface.checkViewAllImages, SIGNAL( clicked( bool ) ), this, SLOT( viewAllImagesClicked() ) );
	m_Interface.checkViewAllImages->setChecked( m_ViewerCore->getSettings()->getPropertyAs<bool>( "viewAllImagesInStack" ) );
	connect( m_Interface.checkViewAllImages, SIGNAL( clicked( bool ) ), this, SLOT( viewAllImagesClicked() ) );

	m_ImageStack->clear();
	ImageHolder::Vector imageList;

	if( m_ViewerCore->hasImage() ) {
		if( m_Interface.checkViewAllImages->isChecked() ) {
			imageList = m_ViewerCore->getImageList() ;
		} else {
			imageList = m_ViewerCore->getUICore()->getCurrentEnsemble()->getImageList();
		}

		BOOST_FOREACH( ImageHolder::Vector::const_reference image, imageList ) {
			if( !( m_ViewerCore->getMode() == ViewerCoreBase::statistical_mode && image->getImageProperties().imageType == ImageHolder::structural_image ) ) {
				QListWidgetItem *item = new QListWidgetItem;
				QString sD = image->getPropMap().getPropertyAs<std::string>( "sequenceDescription" ).c_str();
				item->setText( QString( image->getImageProperties().fileName.c_str() ) );
				item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable );
				item->setData( Qt::UserRole, QVariant( image->getImageProperties().fileName.c_str() ) );

				if( image->getImageProperties().isVisible ) {
					item->setCheckState( Qt::Checked );
				} else {
					item->setCheckState( Qt::Unchecked );
				}
				if ( m_Interface.checkViewAllImages->isChecked() ) {
					const ImageHolder::Vector iList = m_ViewerCore->getUICore()->getCurrentEnsemble()->getImageList();
					if( std::find( iList.begin(), iList.end(), image ) != iList.end() ) {
						item->setBackgroundColor( color::currentEnsemble );
					}
				}
				if( m_ViewerCore->getCurrentImage().get() == image.get() ) {
					item->setIcon( QIcon( ":/common/currentImage.gif" ) );
					item->setTextColor( color::currentImage );
				}

				m_ImageStack->addItem( item );
			}
		}
	}
	if( m_ViewerCore->getUICore()->getEnsembleList().size() > 1 ) {
		m_Interface.checkViewAllImages->setVisible( true );
		m_Interface.moveDown->setVisible( true );
		m_Interface.moveUp->setVisible( true );

		if ( m_ImageStack->currentItem() ) {
			m_Interface.moveUp->setEnabled( checkEnsembleCanUp( getEnsembleFromItem( m_ImageStack->currentItem() ) ) );
			m_Interface.moveDown->setEnabled( checkEnsembleCanDown( getEnsembleFromItem( m_ImageStack->currentItem() ) ) );
		} else {
			m_Interface.moveDown->setEnabled( false );
			m_Interface.moveUp->setEnabled( false );
		}
	} else {
		m_Interface.moveDown->setVisible(false );
		m_Interface.moveUp->setVisible( false );
		m_Interface.checkViewAllImages->setVisible( false );
		
	}

}

void ImageStackWidget::itemClicked ( QListWidgetItem */*item*/ )
{
	if( m_ViewerCore->hasImage() ) {

		if ( m_ViewerCore->getUICore()->getEnsembleList().size() > 1 && m_ImageStack->currentItem() ) {
			m_Interface.moveUp->setEnabled( checkEnsembleCanUp( getEnsembleFromItem( m_ImageStack->currentItem() ) ) );
			m_Interface.moveDown->setEnabled( checkEnsembleCanDown( getEnsembleFromItem( m_ImageStack->currentItem() ) ) );
		} else {
			m_Interface.moveDown->setEnabled( false );
			m_Interface.moveUp->setEnabled( false );
		}
	}
}

void ImageStackWidget::itemChanged( QListWidgetItem *item )
{
	if( m_ViewerCore->hasImage() ) {
		const ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( item->data( Qt::UserRole ).toString().toStdString() );

		if( item->checkState() == Qt::Checked ) {
			image->getImageProperties().isVisible = true ;
		} else {
			image->getImageProperties().isVisible = false ;
		}

		m_ViewerCore->getUICore()->refreshUI( false ); //no update of the mainwindow is needed here
		m_ViewerCore->updateScene();
	}

}

void ImageStackWidget::itemSelected( QListWidgetItem *item )
{
	m_ViewerCore->setCurrentImage( m_ViewerCore->getImageMap().at( item->data( Qt::UserRole ).toString().toStdString() ) );
}

void ImageStackWidget::closeAllImages()
{
	//ok we assume that "close all images" actually means to close all images - not only those that are listed by the imagestack
	ImageHolder::Vector cp = m_ViewerCore->getImageList();
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, cp ) {
		m_ViewerCore->closeImage( image, false ); //do not refresh the ui with each close
	}
	m_ViewerCore->getUICore()->refreshUI();
	LOG_IF( !m_ViewerCore->getUICore()->getEnsembleList().empty(), Dev, error ) << "Closed all images. But the amount of widget ensembles is not 0 ("
			<< m_ViewerCore->getUICore()->getEnsembleList().size() << ") !";
	LOG_IF( !m_ViewerCore->getImageList().empty(), Dev, error ) << "Closed all images. But there are still "
			<< m_ViewerCore->getImageList().size() << " in the global image list!";

}


void ImageStackWidget::closeImage()
{
	if(  m_ImageStack->currentItem() ) {
		m_ViewerCore->closeImage( m_ViewerCore->getImageMap().at( m_ImageStack->currentItem()->data( Qt::UserRole ).toString().toStdString() ) );
	}
}

void ImageStackWidget::distributeImages()
{
	m_ViewerCore->getUICore()->closeAllWidgetEnsembles();
	BOOST_FOREACH( ImageHolder::Vector::const_reference image, m_ViewerCore->getImageList() ) {
		m_ViewerCore->getUICore()->createViewWidgetEnsemble( m_ViewerCore->getSettings()->getPropertyAs<std::string>( "defaultViewWidgetIdentifier" ), image );
	}
	LOG_IF( m_ViewerCore->getImageList().size() != m_ViewerCore->getUICore()->getEnsembleList().size(), Dev, error ) << "Distributed the images. But amount of images ("
			<< m_ViewerCore->getImageList().size() << ") and amount of widget ensembles (" << m_ViewerCore->getUICore()->getEnsembleList().size()
			<< ") does not coincide!";
	m_ViewerCore->getUICore()->refreshUI( false );
	m_ViewerCore->updateScene();
	m_ViewerCore->settingsChanged();
}

void ImageStackWidget::moveDown()
{	
	const WidgetEnsemble::Vector::iterator eIter = std::find(m_ViewerCore->getUICore()->getEnsembleList().begin(), m_ViewerCore->getUICore()->getEnsembleList().end(), getEnsembleFromItem(m_ImageStack->currentItem()));
	if( eIter != m_ViewerCore->getUICore()->getEnsembleList().end() && (eIter+1) != m_ViewerCore->getUICore()->getEnsembleList().end() ) {
		std::iter_swap( eIter, eIter+1 );
	}
	ImageHolder::Vector newImageList;
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference e, m_ViewerCore->getUICore()->getEnsembleList() ) {
		BOOST_FOREACH( ImageHolder::Vector::const_reference imageInE, e->getImageList() ) {
			newImageList.push_back(imageInE);
		}
	}
	m_ViewerCore->getImageList() = newImageList;	
	m_ViewerCore->getUICore()->refreshEnsembles();
	m_ViewerCore->getUICore()->refreshUI();
	

}

void ImageStackWidget::moveUp()
{
	const WidgetEnsemble::Vector::iterator eIter = std::find(m_ViewerCore->getUICore()->getEnsembleList().begin(), m_ViewerCore->getUICore()->getEnsembleList().end(), getEnsembleFromItem(m_ImageStack->currentItem()));
	if( eIter != m_ViewerCore->getUICore()->getEnsembleList().begin() ) {
		std::iter_swap( eIter, eIter-1 );
	}
	ImageHolder::Vector newImageList;
	BOOST_FOREACH( WidgetEnsemble::Vector::const_reference e, m_ViewerCore->getUICore()->getEnsembleList() ) {
		BOOST_FOREACH( ImageHolder::Vector::const_reference imageInE, e->getImageList() ) {
			newImageList.push_back(imageInE);
		}
	}
	m_ViewerCore->getImageList() = newImageList;
	m_ViewerCore->getUICore()->refreshEnsembles();
	m_ViewerCore->getUICore()->refreshUI();
}

bool ImageStackWidget::checkEnsembleCanDown ( const WidgetEnsemble::Pointer ensemble )
{
	const WidgetEnsemble::Vector eL = m_ViewerCore->getUICore()->getEnsembleList();
	return std::find( eL.begin(), eL.end(), ensemble ) != eL.end() - 1;
}

bool ImageStackWidget::checkEnsembleCanUp ( const WidgetEnsemble::Pointer ensemble )
{
	const WidgetEnsemble::Vector eL = m_ViewerCore->getUICore()->getEnsembleList();
	return std::find( eL.begin(), eL.end(), ensemble ) != eL.begin();
}

const WidgetEnsemble::Pointer ImageStackWidget::getEnsembleFromItem ( const QListWidgetItem *item )
{
	const ImageHolder::Pointer image = m_ViewerCore->getImageMap().at( item->data( Qt::UserRole ).toString().toStdString() );
	return m_ViewerCore->getUICore()->getEnsembleFromImage( image );
}


}
}
}