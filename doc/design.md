# 多文档编辑器设计方案

## 0.总览

### 0.1 Manager-Plugins组成方法：

由一个主体(Manager)来管理插件以及插件间的交互，每个插件负责具体的工作。

### 0.2 项目结构

```
mde
|-src
|   |-app
|   |-libs
|   |   |-utils
|   |   |-extensionsystem
|   |   |-editcomponent
|   |   |-mdegui
|   |-plugins
|   |   |-texteditor
|-doc
|-share
```

## 1.应用本体 app

### 1.1 应用程序类 `MdeApplication`

使程序单例化。

使用 `run()->int` 取代主函数，方便使用信号与槽机制。也就是说

```cpp
int main(int argc, char *argv[])
{
    MdeApplication app(argc, argv);
    return app.run();
}
```

派生自 `QApplication`

### 1.2 日志记录器 `Logger`

记录日志。

## 2.扩展系统 extensionsystem

接口类（抽象基类）：插件接口

导出类包括插件管理器、插件特性、全局设置

依赖于 utils

### 2.0 插件接口 `IPlugin`

所有插件的接口。

#### 2.0.1 插件元数据结构

```
{
    "Name" : string,            //插件名
    "Version" : string,         //插件版本
    "CompatVer" : string,       //兼容插件版本
    "Vendor" : string,          //开发者
    "Copyright" : string,       //版权
    "License" : string,         //协议
    "Category" : string,        //插件分类
    "Description" : string,     //插件描述
    "Help" : string,            //命令行参数帮助
    "DisabledByDefault" : bool, //默认禁用，默认false
    "Dependencies" : [          //依赖关系
    {"Name" : string,             //被依赖的插件名
     "Version" : string,          //被依赖的插件版本
     "Optional" : bool            //可选依赖，默认false
    },...]
}
```

#### 2.0.2 接口

```cpp
enum InitializeState {
    Failed = 0,
    Successful = 1,
    Finished = 0xffffffff
};
```

描述初始化状态。

* `initialize(PluginManager::ProgramPeriod period)->InitializeState [pure virtual]`

按阶段初始化。

程序阶段 `Starting, CommandLineParsed, SettingsRead, EventLoop, AboutToQuit` 不会调用该函数，因而也不用实现这几种情况。
插件加载完成 `PluginsLoaded` 时，发生第一次初始化。
如果需要的话，插件加载翻译文件应该发生在 `TranslationLoaded` 阶段。
而界面相关的初始化，应该发生在 `GuiSetup` 阶段。

当该函数返回 `Finished` 后，其对应的 `PluginSpec` 会将状态标记为 `Initialized` 。
如果在进入事件循环之前还没有进入 ` Initialized` 状态，则 `PluginSpec` 会标记一个错误。

* `errorString()->QString [pure virtual]`

返回错误信息。

* `attempToShutdown()->bool [pure virtual]`

尝试关闭插件。

### 2.1 插件管理器 `PluginManager`

管理插件使用状态以及依赖关系。

插件管理器管理一个对象池，可以访问、添加、移除池中对象。
对象池**不**拥有插件接口对象（`IPlugin` 的派生类对象），而是拥有插件的功能对象。
注意对象池不夺取对象的所有权，也不管理对象的生存期。

目前打算用信号-槽向插件传递命令行参数。

单例，不可继承。

#### 2.1.1 公共接口

```cpp
enum ProgramPeriod {
    Starting,          //插件管理器启动
    CommandLineParsed, //命令行参数解析完成
    SettingsRead,      //设置读取完成
    PluginsLoaded,     //插件加载完成
    TranslationLoaded, //翻译文件加载完成
    GuiSetup,          //界面构建完成
    EventLoop,         //事件循环
    AboutToQuit        //即将退出
};
```

描述程序所处的阶段。

* `instance()->PluginManager* [static]`

获取单例实例。

* `addObject(QObject *obj) [static]`

向对象池中添加对象。

线程安全。

* `removeObject(QObject *obj) [static]`

从对象池中移除对象。

线程安全。

* `template<typename T> getObject(const function<bool(const T*)>& filter = [](const T*){return true;})->T* [static]`

从对象池中获取特定类型且满足特定要求的某个对象。

线程安全。

* `template<typename T> getObjects(const function<bool(const T*)>& filter = [](const T*){return true;})->QList<T*> [static]`

从对象池中获取特定类型且满足特定要求的所有对象。

线程安全。

* `getObjectByName(const QString& name)->QObject* [static]`

按名字从对象池中获取对象。

线程安全。

* `objectAdded(QObject*) [signal]`

向对象池中添加对象时发射该信号。

* `aboutToRemoveObject(QObject*) [signal]`

从对象池中移除对象时发射该信号。

* `plugins()->QList<PluginSpec*> [static]`

返回所有在路径中可以找到的插件的插件规格列表。

* `loadOrderQueue()->QList<PluginSpec*> [static]`

按插件加载顺序排列的插件规格列表。所有插件 `Resolved` 后就可以访问。

* `pluginsRequiredByPlugin(const PluginSpec* spec)->QSet<PluginSpec*> [static]`

返回插件规格 `spec` 对应插件（直接或间接）需要的所有插件的插件规格。

* `pluginsRequiringPlugin(const PluginSpec* spec)->QSet<PluginSpec*> [static]`

返回（直接或间接）需要插件规格 `spec` 对应插件的所有插件的插件规格。

#### 2.1.2 私有接口

* `allObjects()->QList<QPointer<QObject>>& [static]`

对象池封装。

* `listLock()->QReadWriteLock& [static]`

读写锁封装。用来保证线程安全。

### 2.2 插件规格 `PluginSpec`

访问插件的元信息、插件状态等的只读接口。

只能由插件管理器 `PluginManager` 创建。

#### 2.2.1 辅助类及辅助函数

```cpp
class Version {
    Version() = delete;
public:
    Version(const QString& ver);
    bool isValid() const;
    QString toString() const;
    bool operator == (const Version& other) const;
    bool operator != (const Version& other) const;
    bool operator >= (const Version& other) const;
    bool operator <= (const Version& other) const;
    bool operator > (const Version& other) const;
    bool operator < (const Version& other) const;
private:
    static constexpr std::size_t level = 4;
    bool vaild;
    quint16 number[level]; //major.minor.patch.build
}
```

版本号辅助类。

```cpp
struct PluginDependency {
    QString name;
    Version version;
    bool optional;
};
```

插件依赖基本信息。

* `qHash(const PluginDependency& value)->uint`

依赖信息的散列函数。

#### 2.2.2 公共接口

```cpp
enum State {
    Invalid,     //起始状态，什么都还没做
    Read,        //读取了插件元数据
    Resolved,    //解析了插件依赖关系
    Loaded,      //加载了插件文件
    Initialized, //完成了初始化
    Running,     //插件运行中
    Terminated   //插件终止
};
```

描述插件状态。

状态 `Read, Resolved` 发生在 `CommandLineParsed` 之前；
状态 `Loaded` 发生在 `SettingsRead` 之后 `PluginsLoaded` 之前；
状态 `Initialized` 最晚发生在 `EventLoop` 之前。

* `state()->State const`

返回插件状态。

* `hasError()->bool const`

是否有错误。

* `errorString()->QString const`

返回错误信息。

* `name()->QString const` 插件名称
* `version()->Version const` 插件版本
* `compatVersion()->Version const` 兼容插件版本
* `vendor()->QString const` 开发者
* `copyright()->QString const` 版权
* `license()->QString const` 协议
* `category()->QString const` 插件分类
* `description()->QString const` 插件描述
* `help()->QString const` 命令行参数帮助
* `isDisabledByDefault()->bool const` 是否默认禁用
* `dependencies()->QVector<PluginDependency> const` 依赖关系

各种插件元数据访问。`Read` 后即可访问。

* `provides(const PluginDependency& depend)->bool const`

检验该插件是否能够作为 `depend` 要求的依赖。`Read` 后即可访问。

* `isForceDisabled()->bool const`
* `isForceEnabled()->bool const`

是否由命令行或设置等导致的强制禁用/启用。

* `dependencySpecs()->QHash<PluginDependency, PluginSpec*> const`

该插件**直接**依赖的插件。`Resolved` 后即可访问。

* `requiresAnyOf(const QSet<PluginSpec*>& plugins)->bool const`

该插件是否**必须**依赖于 `plugins` 中的某一个。`Resolved` 后即可访问。

* `plugin()->IPlugin*  const`

获取该插件的实例。`Loaded` 后即可访问。

### 2.3 全局设置 `GlobalSettings`

全局设置的统一访问接口。

单例，不可继承。

#### 2.3.1 公共接口

* `instance()->GlobalSetting* [static]`

返回单例实例。

* `[property] bool enabled`

是否启用全局设置。

* `setDynamicSetting(const QString& namespace, const QString& key, const QVariant& value)`

设置动态设置。

* `queryDynamicSetting(const QString& namespace, const QString& key)->QVariant`

查询动态设置。

## 3.编辑组件 editcomponent

编辑器相关组件。

导出类包括编辑器管理器、编辑器工厂基类、编辑器基类

依赖于 extensionsystem, mdegui

### 3.1 编辑器管理器 `EditorManager`

编辑器工厂的管理。

编辑器管理器管理一个从 `MimeType` 到编辑器工厂的映射，控制默认状态下打开文件所选择的编辑器。

单例，不可继承。

#### 3.1.1 公共接口

* `instance()->EditorManager* [static]`

返回单例实例。

* `new() [static]`

* `newWith(BaseEditorFactory* editor) [static]`

* `open(const QString& fileName) [static]`

* `openWith(const QString& fileName, BaseEditorFactory* editor) [static]`

* `saveCurrent() [static]`

* `saveCurrentAs(const QString& fileName) [static]`

* `saveAll() [static]`

* `currentFile()->QFileInfo [static]`

* `setDefaultEditor(const QMimeType& mimeType, const BaseEditorFactory* editor) [static]`

* `queryDefaultEditor(const QMimeType& mimeType)->BaseEditorFactory* [static]`

* `queryDefaultEditor(const QString& fileName)->BaseEditorFactory* [static]`

### 3.2 编辑器工厂基类 `BaseEditorFactory`

编辑器工厂创建编辑器实例。

#### 3.2.1 公共接口

* `new()->BaseEditor*  [virtual]`

* `openFile(const QString& fileName)->BaseEditor* [virtual]`

### 3.3 编辑器基类 `BaseEditor`

#### 3.3.1 公共接口

* `editWidget()->QWidget* [virtual]`

* `clear() [virtual]`

* `loadFile(const QString& fileName) [virtual]`

* `file()->QFileInfo [virtual]`

* `save() [virtual]`

* `saveAs(const QString& fileName) [virtual]`

* `isActive()->bool [virtual]`

#### 3.3.2 保护接口

* `setFile(const QFileInfo& file) [virtual]`

* `setActive(bool active) [virtual]`

## 4.图形界面 mdegui

图形界面控制。

导出类：界面管理器

依赖于 extensionsystem

### 4.1 界面管理器 `MdeManager`

管理窗口、标签页等界面元素。

目前打算让窗口不能直接从界面管理器以外访问（有待进一步考量）

单例，不可继承。

### 4.1.1 公共接口

* `instance()->MdeManager* [static]`

返回单例实例。

### 4.1.2 主窗口布局层次：

```
QVBoxLayout
|-EditWidget
|-QHBoxLayout
    |-MenuButton : (Fix,Fix)
    |-QVBoxLayout
        |-ToolBar : (Expand,Fix)
        |-TabBar : (Expand,Fix)
```

## 5.工具库 utils

杂项工具。

命令行相关：

### 5.1 命令行解析器 `CommandLineParser`



## N.杂七杂八
