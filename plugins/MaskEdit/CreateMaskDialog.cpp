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
 * CreateMaskDialog.cpp
 *
 * Description:
 *
 *  Created on: Aug 12, 2011
 *      Author: tuerke
 ******************************************************************/
#include "CreateMaskDialog.hpp"


namespace isis
{
namespace viewer
{
namespace plugin
{

CreateMaskDialog::CreateMaskDialog( QWidget *parent, MaskEditDialog *maskEditDialog )
	: QDialog( parent ),
	  m_MaskEditDialog( maskEditDialog )
{
	m_Interface.setupUi( this );
	connect( m_Interface.createButton, SIGNAL( clicked() ), this, SLOT( createMask() ) );
	m_Interface.maskName->setText( "mask1" );
	m_Interface.xRes->setMinimum( 0.01 );
	m_Interface.yRes->setMinimum( 0.01 );
	m_Interface.zRes->setMinimum( 0.01 );
	m_Interface.xRes->setMaximum( 10 );
	m_Interface.yRes->setMaximum( 10 );
	m_Interface.zRes->setMaximum( 10 );
}

void CreateMaskDialog::showEvent( QShowEvent * )
{
	if( m_MaskEditDialog->m_ViewerCore->hasImage() ) {
		m_Interface.maskType->clear();
		BOOST_FOREACH( std::list<std::string>::const_reference type, isis::viewer::getSupportedTypeList() ) {
			m_Interface.maskType->addItem( type.c_str() );
		}

		if( m_MaskEditDialog->m_ViewerCore->hasImage() ) {
			const util::fvector4 &voxelSize = m_MaskEditDialog->m_ViewerCore->getCurrentImage()->getISISImage()->getPropertyAs<util::fvector4>( "voxelSize" ) ;
			m_Interface.xRes->setValue( voxelSize[0] );
			m_Interface.yRes->setValue( voxelSize[1] );
			m_Interface.zRes->setValue( voxelSize[2] );
		} else {
			m_Interface.xRes->setValue( 3 );
			m_Interface.yRes->setValue( 3 );
			m_Interface.zRes->setValue( 3 );
		}
	}
}


void CreateMaskDialog::createMask()
{
	if( m_MaskEditDialog->m_ViewerCore->hasImage() ) {
		boost::shared_ptr<ImageHolder> refImage = m_MaskEditDialog->m_ViewerCore->getCurrentImage();
		std::string dataType = m_Interface.maskType->currentText().toStdString();
		dataType.append( "*" );
		boost::shared_ptr<ImageHolder> maskImage;

		switch ( isis::util::getTransposedTypeMap( false, true ).at( dataType ) ) {
		case isis::data::ValuePtr<bool>::staticID:
			maskImage = _createEmptyMask<bool>( refImage );
			break;
		case isis::data::ValuePtr<int8_t>::staticID:
			maskImage = _createEmptyMask<int8_t>( refImage );
			break;
		case isis::data::ValuePtr<uint8_t>::staticID:
			maskImage = _createEmptyMask<uint8_t>( refImage );
			break;
		case isis::data::ValuePtr<int16_t>::staticID:
			maskImage = _createEmptyMask<int16_t>( refImage );
			break;
		case isis::data::ValuePtr<uint16_t>::staticID:
			maskImage = _createEmptyMask<uint16_t>( refImage );
			break;
		case isis::data::ValuePtr<int32_t>::staticID:
			maskImage = _createEmptyMask<int32_t>( refImage );
			break;
		case isis::data::ValuePtr<uint32_t>::staticID:
			maskImage = _createEmptyMask<uint32_t>( refImage );
			break;
		case isis::data::ValuePtr<int64_t>::staticID:
			maskImage = _createEmptyMask<int64_t>( refImage );
			break;
		case isis::data::ValuePtr<uint64_t>::staticID:
			maskImage = _createEmptyMask<uint64_t>( refImage );
			break;
		case isis::data::ValuePtr<float>::staticID:
			maskImage = _createEmptyMask<float>( refImage );
			break;
		case isis::data::ValuePtr<double>::staticID:
			maskImage = _createEmptyMask<double>( refImage );
			break;
		default:
			LOG( Runtime, error )  << "Unknown type " << dataType << " !";
			return;
		}

		m_MaskEditDialog->m_CurrentMask = maskImage;
		m_MaskEditDialog->m_CurrentMask->extent = m_MaskEditDialog->m_CurrentMask->minMax.second->as<double>() -  m_MaskEditDialog->m_CurrentMask->minMax.first->as<double>();
		m_MaskEditDialog->m_CurrentMask->opacity = 0.5;
		m_MaskEditDialog->m_CurrentMask->lut = "maskeditLUT";
		m_MaskEditDialog->m_CurrentMask->updateColorMap();
		m_MaskEditDialog->m_CurrentMask->updateOrientation();
		BOOST_FOREACH( UICore::ViewWidgetEnsembleListType::const_reference ensemble, m_MaskEditDialog->m_ViewerCore->getUICore()->getEnsembleList() ) {
			WidgetInterface::ImageVectorType iVector;

			for( unsigned short i = 0; i < 3; i++ ) {
				iVector = ensemble[i].widgetImplementation->getImageVector();

				if( std::find( iVector.begin(), iVector.end(), refImage ) != iVector.end() ) {
					m_MaskEditDialog->m_CurrentWidgetEnsemble = ensemble;
					m_MaskEditDialog->m_ViewerCore->attachImageToWidget( m_MaskEditDialog->m_CurrentMask, ensemble[i].widgetImplementation ) ;
					ensemble[i].widgetImplementation->setMouseCursorIcon( QIcon( ":/common/paintCrosshair.png" ) );

				}
			}
		}
		m_MaskEditDialog->m_ViewerCore->setCurrentImage( m_MaskEditDialog->m_CurrentMask );
		m_MaskEditDialog->m_ViewerCore->setShowCrosshair( false );
		m_MaskEditDialog->m_ViewerCore->updateScene();
		m_MaskEditDialog->m_ViewerCore->getUICore()->refreshUI();
		m_MaskEditDialog->m_Interface.cut->setEnabled( true );
		m_MaskEditDialog->m_Interface.paint->setEnabled( true );
		m_MaskEditDialog->m_Interface.radius->setEnabled( true );
		m_MaskEditDialog->m_Interface.paint->setChecked( true );
	}

	close();
}



}
}
}