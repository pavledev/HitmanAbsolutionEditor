#pragma once

#include "BasePanel.h"
#include "Rendering/Renderer3D.h"

class ModelViewerPanel : public BasePanel
{
public:
	ModelViewerPanel(const char* name, const char* icon);
	void Render() override;
	void OnResourceLoaded();
	std::shared_ptr<Renderer3D> GetRenderer3D() const;

private:
	std::shared_ptr<Renderer3D> renderer3D;
	float viewportWidth;
	float viewportHeight;
};
