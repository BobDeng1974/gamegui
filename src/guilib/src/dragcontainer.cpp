#include "StdAfx.h"
#include "dragcontainer.h"

#include "system.h"

namespace gui
{
	DragContainer::DragContainer(System& sys, const std::string& name) :
		WindowBase(sys, name),
		m_dropTarget(0),
		m_dragSubject(0)
	{
		reset();
	}

	void DragContainer::rise()
	{
		if (!m_parent) return;

		children_t& children = m_parent->getChildren();
		child_iter it = std::find(children.begin(), children.end(), ptr());
		if (it == children.end()) return;

		auto temp = *it;
		children.erase(it);
		children.push_back(temp);
	}
	
	void DragContainer::update(WindowBase* target, const point& pt)
	{
		m_dropTarget = target;
		setPosition(pt - m_childOffset);
	}

	void DragContainer::reset(void)
	{
		setVisible(false);
		setIgnoreInputEvents(true);
		m_dropTarget = 0;
		m_dragSubject = 0;
	}

	bool DragContainer::startDrag(WindowBase* subj, const point& off)
	{
		setVisible(true);
		m_dragSubject = subj;
		
		DragEventArgs de;
		de.name = "On_DragStarted";
		de.offset = off;
		de.subj = m_dragSubject;
		subj->callHandler(&de);
		
		m_childOffset = de.offset;
		return de.handled;
	}

	bool DragContainer::stopDrag(void)
	{
		if(!m_dropTarget)
			m_dropTarget = m_system.getRootPtr().get();
		
		if(m_dropTarget->isAcceptDrop())
		{
			DragEventArgs de;
			de.name = "On_DragDropped";
			de.subj = m_dragSubject;
			m_dropTarget->callHandler(&de);
		}
		
		if (!m_system.isDragFrozen())
		{
			DragEventArgs de;
			de.name = "On_DragStopped";
			de.subj = m_dragSubject;
			m_dragSubject->callHandler(&de);

			if (de.handled)
			{
				reset();
				return true;
			}
		}

		return false;
	}
}