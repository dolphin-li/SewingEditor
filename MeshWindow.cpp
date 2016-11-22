#include "MeshWindow.h"
#include "global_data_holder.h"
#include "SC\analysis2d_cloth_static.h"
#include "svgpp\SvgManager.h"
#include "svgpp\SvgPolyPath.h"
MeshWindow::MeshWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);	
	new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(showFullScreen()));
	new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(showNormal()));
	setAcceptDrops(true);
	initLeftDockActions();
	startTimer(200);
}

MeshWindow::~MeshWindow()
{

}

BaseMeshViewer* MeshWindow::getViewer()
{
	return ui.widget;
}

const BaseMeshViewer* MeshWindow::getViewer()const
{
	return ui.widget;
}

void MeshWindow::updateParamUI()
{
	auto clothParam = g_dataholder.m_clothManger->GetParam_Cloth();
	ui.dbStiffBend->setValue(clothParam.stiff_bend);
	ui.dbStiffMyu->setValue(clothParam.stiff_myu);
	ui.dbStiffLambda->setValue(clothParam.stiff_lambda);
	ui.dbRho->setValue(clothParam.rho);

	auto contactParam = g_dataholder.m_clothManger->GetParam_Contact();
	ui.dbMyuK->setValue(contactParam.myu_k);
	ui.dbMyuS->setValue(contactParam.myu_s);
	ui.dbStiffN->setValue(contactParam.stiff_n);
	ui.dbStiffF->setValue(contactParam.stiff_f);
	ui.dbOffset->setValue(contactParam.offset);

	ui.sbCoarseMesh->setValue(g_dataholder.m_param.m_coarse_mesh_ntri);
	ui.sbDetailMesh->setValue(g_dataholder.m_param.m_detail_mesh_ntri);
}

void MeshWindow::timerEvent(QTimerEvent* ev)
{
	float fps = ui.widget->getFps();
	setWindowTitle(QString().sprintf("fps = %.1f", fps));
}

void MeshWindow::dragEnterEvent(QDragEnterEvent* event)
{
	//if (event->mimeData()->hasUrls())
	//{
	//	QList<QUrl> urls = event->mimeData()->urls();
	//	if (urls[0].fileName().endsWith(".svg"))
	//		event->acceptProposedAction();
	//}
}

void MeshWindow::dropEvent(QDropEvent* event)
{
	//QUrl url = event->mimeData()->urls()[0];
	//QString name = url.toLocalFile();
	//try
	//{
	//	if (ui.widget->getSvgManager() == nullptr)
	//		throw std::exception("svgManger: nullptr");
	//	ui.widget->getSvgManager()->load(name.toStdString().c_str());
	//	pushHistory("load svg");
	//	ui.widget->updateGL();
	//}
	//catch (std::exception e)
	//{
	//	std::cout << e.what() << std::endl;
	//}
	//catch (...)
	//{
	//	std::cout << "unknown error" << std::endl;
	//}

	//event->acceptProposedAction();
}

void MeshWindow::initLeftDockActions()
{
	m_ldbSignalMapper.reset(new QSignalMapper(this));
	connect(m_ldbSignalMapper.data(), SIGNAL(mapped(int)), this, SLOT(leftDocButtonsClicked(int)));
	ui.dockWidgetLeftContents->setLayout(new QGridLayout(ui.dockWidgetLeftContents));
	ui.dockWidgetLeftContents->layout()->setAlignment(Qt::AlignTop);

	// add buttons
	for (size_t i = (size_t)AbstractMeshEventHandle::ProcessorTypeClothSelect;
		i < (size_t)AbstractMeshEventHandle::ProcessorTypeEnd; i++)
	{
		auto type = AbstractMeshEventHandle::ProcessorType(i);
		addLeftDockWidgetButton(type);
	}
	m_leftDockButtons[AbstractMeshEventHandle::ProcessorTypeClothSelect]->setChecked(true);
	m_leftDockButtons[AbstractMeshEventHandle::ProcessorTypeClothTranslate]->setShortcut(Qt::Key_T);
	m_leftDockButtons[AbstractMeshEventHandle::ProcessorTypeClothRotate]->setShortcut(Qt::Key_R);

	// do connections
	for (auto it : m_leftDockButtons.toStdMap())
	{
		m_ldbSignalMapper->setMapping(it.second.data(), it.first);
		connect(it.second.data(), SIGNAL(clicked()), m_ldbSignalMapper.data(), SLOT(map()));
	}
}

void MeshWindow::addLeftDockWidgetButton(AbstractMeshEventHandle::ProcessorType type)
{
	auto handle = ui.widget->getEventHandle(type);
	auto colorStr = QString("background-color: rgb(73, 73, 73)");
	QIcon icon;
	icon.addFile(handle->iconFile(), QSize(), QIcon::Active);
	icon.addFile(handle->iconFile(), QSize(), QIcon::Selected);
	icon.addFile(handle->inactiveIconFile(), QSize(), QIcon::Normal);
	QSharedPointer<QPushButton> btn(new QPushButton(ui.dockWidgetLeft));
	btn->setIconSize(QSize(80, 80));
	btn->setIcon(icon);
	btn->setCheckable(true);
	btn->setStyleSheet(colorStr);
	btn->setAutoExclusive(true);
	btn->setToolTip(handle->toolTips());
	m_leftDockButtons.insert(type, btn);
	ui.dockWidgetLeftContents->layout()->addWidget(btn.data());
}

void MeshWindow::leftDocButtonsClicked(int i)
{
	AbstractMeshEventHandle::ProcessorType type = (AbstractMeshEventHandle::ProcessorType)i;
	ui.widget->setEventHandleType(type);
	m_leftDockButtons[type]->setChecked(true);
}

void MeshWindow::on_cbThickness_currentTextChanged(const QString& s)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Cloth();
		QString s1 = s.toLower();
		if (s1 == "thin")
		{
			ui.dbStiffBend->setValue(1e-10);
		}
		else if (s1 == "very thin")
		{
			ui.dbStiffBend->setValue(0);
		}
		else if (s1 == "medium")
		{
			ui.dbStiffBend->setValue(1e-5);
		}
		else if (s1 == "thick")
		{
			ui.dbStiffBend->setValue(1e-2);
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbStiffBend_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Cloth();
		param.stiff_bend = v;
		g_dataholder.m_clothManger->SetParam_Cloth(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbStiffMyu_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Cloth();
		param.stiff_myu = v;
		g_dataholder.m_clothManger->SetParam_Cloth(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbStiffLambda_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Cloth();
		param.stiff_lambda = v;
		g_dataholder.m_clothManger->SetParam_Cloth(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbRho_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Cloth();
		param.rho = v;
		g_dataholder.m_clothManger->SetParam_Cloth(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbMyuK_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Contact();
		param.myu_k = v;
		g_dataholder.m_clothManger->SetParam_Contact(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbMyuS_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Contact();
		param.myu_s = v;
		g_dataholder.m_clothManger->SetParam_Contact(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbStiffN_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Contact();
		param.stiff_n = v;
		g_dataholder.m_clothManger->SetParam_Contact(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbStiffF_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Contact();
		param.stiff_f = v;
		g_dataholder.m_clothManger->SetParam_Contact(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_dbOffset_valueChanged(double v)
{
	try
	{
		auto param = g_dataholder.m_clothManger->GetParam_Contact();
		param.offset = v;
		g_dataholder.m_clothManger->SetParam_Contact(param);
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_sbCoarseMesh_valueChanged(int v)
{
	try
	{
		g_dataholder.m_param.m_coarse_mesh_ntri = v;
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_sbDetailMesh_valueChanged(int v)
{
	try
	{
		g_dataholder.m_param.m_detail_mesh_ntri = v;
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_pbReInitMesh_clicked()
{
	try
	{
		g_dataholder.svgToCloth();
		updateParamUI();
		ui.widget->updateGL();
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}

void MeshWindow::on_pbInverseCylinder_clicked()
{
	try
	{
		if (ui.widget->pAnalysis()->GetMode() == CLOTH_INITIAL_LOCATION)
		{
			ldp::Double3 o, u, v;
			int id_l;
			if (ui.widget->getEventHandle(ui.widget->getEventHandleType())->getPickedMeshFrameInfo(o, u, v, id_l))
			{
				ui.widget->pAnalysis()->getClothHandle().inverseCylinder(id_l);
				ui.widget->pAnalysis()->SetClothPiecePlacingMode();
				auto iter = g_dataholder.m_clothLoopId2svgIdMap.find(id_l);
				if (iter != g_dataholder.m_clothLoopId2svgIdMap.end())
				{
					auto obj = g_dataholder.m_svgManager->getObjectById(iter->second);
					if (obj == nullptr)
						throw std::exception("error: loop_to_svg index not valid!");
					if (obj->objectType() != svg::SvgAbstractObject::PolyPath)
						throw std::exception("error: loop_to_svg index not valid!");
					auto poly = (svg::SvgPolyPath*)obj;
					poly->setCylinderDir(ui.widget->pAnalysis()->getClothHandle().isCylinderInversed(id_l));
				} // end if iter
			}
		} // end if initial_location
	} catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "unknown error" << std::endl;
	}
}
