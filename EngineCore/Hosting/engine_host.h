#pragma once

#include "Modularization/module_manager.h"
#include "DependencyInjection/service_provider.h"

namespace Engine
{
namespace Core
{
namespace Hosting
{

/// <summary>
/// The engine runtime object.
/// Construct this thing and kick off the game.
/// </summary>
class EngineHost
{
private:
	Modularization::ModuleManager m_ModuleManager;
	DependencyInjection::ServiceProvider m_ServiceProvider;

	// configurations
public:
	/// <summary>
	/// Register a sink module explicitly.
	/// </summary>
	/// <param name="create">Allocate the module state object, and return its pointer.</param>
	/// <param name="update">Run update by directly interface with engine data.</param>
	/// <param name="finalize">Deallocate the module state object, and run any internal logic to free resources.</param>
	/// <param name="name">Name of the module. It's not used as an UUID (subject to change)</param>
	void AddSinkModule(void* (*create)(DependencyInjection::ServiceProvider* services), void (*update)(void* moduleInstance, DependencyInjection::ServiceProvider* services), void (*finalize)(void* moduleInstance, DependencyInjection::ServiceProvider* services), const std::string& name);

	/// <summary>
	/// <para>
	///	Register a sink module. Sink modules run at the last stage of the update loop, and semantically they don't mutate gameplay state.
	/// An example for a sink module is a rendering module, or an audio module.
	/// </para>
	/// The module class supplied should have the following static members:
	/// <list type="number">
	/// <item>
	/// 	<term>void* Create(ServiceProvider* services)</term>
	/// 	<description>Allocate the module state object, and return its pointer.</description>
	/// </item>
	/// <item>
	/// 	<term>void Update(void* moduleInstance, ServiceProvider* services)</term>
	/// 	<description>Run update by directly interface with engine data.</description>
	/// </item>
	/// <item>
	/// 	<term>void Finalize(void* moduleInstance, DependencyInjection::ServiceProvider* services)</term>
	/// 	<description>Deallocate the module state object, and run any internal logic to free resources.</description>
	/// </item>
	/// <item>
	/// 	<term>std::string Name</term>
	/// 	<description>Name of the module. It's not used as an UUID (subject to change)</description>
	/// </item>
	/// </list>
	/// </summary>
	/// <typeparam name="TSinkModule">Module implementation class.</typeparam>
	template <typename TSinkModule>
	void AddSinkModule()
	{
		AddSinkModule(TSinkModule::Create, TSinkModule::Update, TSinkModule::Finalize, TSinkModule::Name);
	}

public:
	/// <summary>
	/// Run the configured game.
	/// </summary>
	/// <returns>Game's return status.</returns>
	int Run();
};

}
}
}
