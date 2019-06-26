#pragma once

class IResizable {

public:
	virtual ~IResizable() {}

	virtual void resize(int newWidth, int newHeight) = 0;
};