#   接口设计说明

## 插件基本接口 (IPluginBase)

插件基本接口通过一个抽象基类IPluginBase声明，通过继承该基类以及QObject并实现其方法来制作一个插件。

```cpp
class IPluginBase
{
public:
    virtual ~IPluginBase() { }
    virtual bool initialize(GeneralSettings * settings) = 0;
    virtual void setMDE(MdeWindow * w) = 0;
};
```

另外，每个插件应该使用一个JSON文件储存插件元数据(MetaData)，这个文件应当具有如下形式：

```json
{
    "name" :  "Hello World",
    "version" : "1.0",
    "vendor" : "miroox",
    "platforms" : "all"
}
```

其中，`"name"`是插件名，`"version"`是插件版本号，`"vendor"`是开发商/制作者，`"platforms"`是其适用的平台。

注意这些元数据只是该插件的一个标识(spec)，用于区分不同的插件，并无实际作用。

---

### `bool IPluginBase::initialize(GeneralSettings * settings)`

此函数用于初始化一些基本属性和设置，而图形界面不应当在这里初始化。（如果需要）翻译文件应当在这里加载。

如果初始化成功，此函数应当返回true，否则，返回false。

此函数的调用紧接在插件管理器加载此插件后。

### `void IPluginBase::setMDE(MdeWindow * w)`

此函数获取主窗口指针，并初始化图形界面相关部分。

此函数调用发生在主窗口构造完成之后，显示之前。

## 编辑器接口 (IEditor)

编辑器由编辑器插件来创建，在这里，编辑器插件类似于一个工厂(factory)。

编辑器插件也是插件，因此也需要继承QObject和IPluginBase，另外，还需要继承抽象基类IEditorPlugin并实现。

```cpp
class IEditorPlugin
{
public:
    virtual ~IEditorPlugin() { }
    virtual IEditor * create() = 0;
    virtual QStringList designedTypes() const = 0;
    virtual QString typeDescription(QString suffix) const = 0;
};
```

编辑器类应当继承IEditor以及QObject，以便使用Qt信号/槽机制。

```cpp
class IEditor
{
public:
    virtual ~IEditor() { }
    virtual IEditorPlugin * base() const = 0;
    virtual QWidget * widget() const = 0;
    virtual void extraInitialize(MdiSubWindow *tab) = 0;
    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual void newFile() = 0;
    virtual void loadFile(QString fileName) = 0;
    virtual void reload() = 0;
    virtual void save() = 0;
    virtual void saveAs(QString fileName) = 0;
    virtual QString title() const = 0;
    virtual QFileInfo file() const = 0;
    virtual bool close() = 0;
};
```

---

### `IEditor * IEditorPlugin::create()`

创建一个编辑器实例，返回它的指针。

此函数调用发生在任何需要此类型编辑器的时候，例如：新建或打开。

### `QStringList IEditorPlugin::designedTypes() const`

返回此类编辑器支持的文件类型（后缀名）。

### `QString IEditorPlugin::typeDescription(QString suffix) const`

返回此类编辑器对后缀名suffix的类型描述，用于文件对话框的筛选器(filter)。

---

### `IEditorPlugin * IEditor::base() const`

返回创建此编辑器的编辑器插件的指针。

### `QWidget * IEditor::widget() const`

返回编辑器窗口部件，它将被设置到子窗口上。

### `void IEditor::extraInitialize(MdiSubWindow *tab)`

根据所设置到的子窗口(tab)，进行额外的初始化。

此函数调用发生在上述部件设置到子窗口之后。

### `void IEditor::activate()`

当该编辑器对应的子窗口被激活时被调用。

例如可以将需要的动作(action)添加到菜单栏或工具栏中。

一个可能的优化是检查上次激活的是否是同种编辑器，利用类静态成员或者base()。

### `void IEditor::deactivate()`

当该编辑器对应的子窗口被停用时被调用。

应该执行和activate()相反的任务。

### `void IEditor::newFile()`

新建一个空文档

### `void IEditor::loadFile(QString fileName)`

加载文件fileName。

成功了应当使主窗口发射一个openedFile(QString)信号，建议使用信号连信号的方式实现。相反，失败时应该使主窗口发射openFailed(QString)信号。注意信号参数应该是一个native canonical file path (例如QDir::toNativeSeparators(file().canonicalFilePath()))。

建议：实际加载文件时使用并发。

### `void IEditor::reload()`

重新加载当前文件。

与loadFile()类似，成功/失败应当使主窗口发射相应信号。

建议：实际加载文件时使用并发。

### `void IEditor::save()`

保存当前文件。

成功了应当使主窗口发射一个savedFile(QString)信号，建议使用信号连信号的方式实现。相反，失败时应该使主窗口发射saveFailed(QString)信号。注意信号参数应该是一个native canonical file path (例如QDir::toNativeSeparators(file().canonicalFilePath()))。

要求：实际保存文件时使用并发。

### `void IEditor::saveAs(QString fileName)`

将当前文档另存为fileName。

与save()类似，成功/失败应当使主窗口发射相应信号。

成功后应当使编辑器的当前文件变成fileName。

建议：实际保存文件时使用并发。

### `QString IEditor::title() const`

返回当前文档标题，通常是文件名。

### `QFileInfo IEditor::file() const`

返回当前文件信息。

如果当前没有确定文件（例如，一个新建文档），则返回空的QFileInfo对象。

### `bool IEditor::close()`

关闭编辑器。如果成功返回true，否则，返回false。

如果发生了修改，关闭编辑器前应询问是否保存。如果需要保存，则应该调用编辑器对应子窗口的MdiSubWindow::save()。

