#if !defined(MPEG1_PICTUREQUEUE_H)
#define MPEG1_PICTUREQUEUE_H

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

#include "picture.h"

namespace Mpeg1
{
	class PictureQueue
	{
	public:
		void put(Picture *picture) 
		{
			m_queueMutex.lock();
			m_queue.append(picture);
			m_queueMutex.unlock();

			m_waitCondition.wakeOne();
		}

		Picture *get()
		{
			m_queueMutex.lock();
			while(m_queue.isEmpty())
			{
				m_waitCondition.wait(&m_queueMutex);
			}
			Picture *picture = m_queue.takeFirst();
			m_queueMutex.unlock();

			return picture;
		}

	private:
		QList<Picture *> m_queue;
		QMutex m_queueMutex;
		QWaitCondition m_waitCondition;
	};
}

#endif