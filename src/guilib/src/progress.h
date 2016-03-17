#pragma once

#include "label.h"

namespace gui
{
	class Image;
	class Imageset;
	typedef std::shared_ptr<Imageset> ImagesetPtr;

	class Progress : public Label
	{
	public:
		typedef Progress self_t;
		Progress(System& sys, const std::string& name = std::string());
		virtual ~Progress();

		static const char* GetType() { return "Progress"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		float getProgress() const { return m_progress; }
		void setProgress(float val)
		{
			m_progress = val;
			invalidate();
		}

		virtual void init(xml::node& node);

	protected:
		float m_progress;
	};

	class ProgressBar : public Progress
	{
	public:
		typedef ProgressBar self_t;
		ProgressBar(System& sys, const std::string& name = std::string());
		virtual ~ProgressBar();

		static const char* GetType() { return "ProgressBar"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);

	protected:
		ImagesetPtr m_imgset;
		
		// progress imagery
		const Image*	m_leftImg;
		const Image*	m_rightImg;
		const Image*	m_backImg;

		bool m_signed;
	};

	class  ImageBar : public Progress
	{
	public:
		typedef ImageBar self_t;
		ImageBar(System& sys, const std::string& name = std::string());
		virtual ~ImageBar(void);

		static const char* GetType() { return "ImageBar"; }
		virtual const char* getType() const { return self_t::GetType(); }

		virtual void render(const Rect& finalRect, const Rect& finalClip);

		virtual void init(xml::node& node);

	protected:
		ImagesetPtr m_imgset;
		
		// progress imagery
		const Image*	m_backImg;
		const Image*	m_foreImg;
	};
}