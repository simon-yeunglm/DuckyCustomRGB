#pragma once

// by simon yeung, 31/10/2020
// all rights reserved

class MacroAction
{
protected:
	MacroAction()= default;

public:
	virtual ~MacroAction()= default;

	// return whether the action is ended
	virtual bool	start();
	virtual bool	update(float deltaSeconds);
	virtual void	end();
};

