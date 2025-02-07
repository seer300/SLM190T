#ifndef __OSAL_H_
#define __OSAL_H_


#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

#include "cmsis_device.h"
#include "osal_config.h"
#include "osal_compiler.h"
#include "osal_type.h"


//  ==== Core Management Functions ====

/**
 * @brief    进入临界区，并临时关闭中断 
 * @warning  关闭中断时间不宜过长，否则会造成底层驱动工作异常，如丢数据等
 */
void osCoreEnterCritical (void);

/**
 * @brief    退出临界区，并恢复中断 
 * @warning  
 */
void osCoreExitCritical (void);

/**
 * @brief    查看内核状态，在响应中断还是处于临界区 
 * @warning
 */
osCoreState_t osCoreGetState (void);


//  ==== Kernel Management Functions ====

/**
 * @brief 初始化RTOS内核,main函数中调用
 * @warning 
*/ 
osStatus_t osKernelInitialize (void);

/**
 * @brief 获取当前内核工作状态
 * @warning 
*/ 
osKernelState_t osKernelGetState (void);

/**
 * @brief    获取内核当前是否在运行idle
 * @warning  
 */
osStatus_t osKernelIsRunningIdle (void);

/**
 * @brief    启动内核，开始线程调度运行，常用于main最后
 * @warning  
 */
osStatus_t osKernelStart (void);

/**
 * @brief    锁住内核线程调度，但不关闭中断。返回当前内核状态，以便后续恢复
 * @warning  一般与osKernelRestoreLock配对使用，而非osKernelUnlock
 * @note    不建议使用该接口！因为该接口开销较大，建议使用osMutexAcquire来进行全局数据的保护
 */
int32_t osKernelLock (void);

/**
 * @brief    根据入参lock，恢复osKernelLock之前的内核状态
 * @warning  必须与osKernelLock配套使用
 */
int32_t osKernelRestoreLock (int32_t lock);

/**
 * @brief    解锁内核调度
 * @warning  该接口强行解锁内核调度，不建议使用
 */
int32_t osKernelUnlock (void);


/**
 * @brief    获取系统当前的tick数，常用于计时，单位为1毫秒，具体值参看osKernelGetTickFreq返回值
 * @warning  osKernelLock锁调度期间，tick数不会被累加
 */
uint32_t osKernelGetTickCount (void);

/**
 * @brief    tick的频率，目前为1毫秒
 */
uint32_t osKernelGetTickFreq (void);


//  ==== Thread Management Functions ====

/**
 * @brief   创建一个线程
 * 
 * @param func      [IN] 线程的入口函数
 * @param argument  [IN] 线程入口函数的参数
 * @param attr      [IN] 线程属性，线程创建之前给结构体赋值
 * typedef struct {
  const char                   *name;   ///< 线程名称
  uint32_t                 attr_bits;   ///< 默认为osThreadDetached
  void                      *cb_mem;    ///< 默认为NULL
  uint32_t                   cb_size;   ///< 默认为0
  void                   *stack_mem;    ///< 默认为NULL
  uint32_t                stack_size;   ///< 申请的栈大小，当线程作用为共享栈时，默认为osStackShared
  osPriority_t              priority;   ///< 线程优先级
  TZ_ModuleId_t            tz_module;   ///< 暂时默认为0
  uint32_t                  reserved;   ///< 必须为0
} osThreadAttr_t;
 
 * @return   创建的线程的id，返回NULL表示创建失败
 * @warning  用户新增的线程优先级建议设置为osPriorityNormal，不得过高！ \n
 * 线程默认不受standby睡眠影响，即会准时进行超时调度。如果客户线程中存在高频率的超时轮询机制，如5秒，则会大大影响功耗，请与芯翼FAE联系。 
 */
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);

/**
 * @brief    根据ID获取线程名
 * @warning  
 */
const char *osThreadGetName (osThreadId_t thread_id);

/**
 * @brief    获取当前工作线程的ID
 * @warning  
 */
osThreadId_t osThreadGetId (void);

/**
 * @brief    根据ID获取某线程当前的状态
 * @warning  
 */
osThreadState_t osThreadGetState (osThreadId_t thread_id);

/**
 * @brief    根据ID获取某线程栈空间大小，单位为字节
 * @warning  接口未实现
 */
uint32_t osThreadGetStackSize (osThreadId_t thread_id);

/**
 * @brief    根据ID获取某线程栈使用的历史峰值，单位为字节
 * @warning  
 */
uint32_t osThreadGetStackSpace (osThreadId_t thread_id);

/**
 * @brief    动态设置某ID线程的调度优先级
 * @param   thread_id   [in]  thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
 * @param   priority    [in]  new priority value for the thread function.
 * @warning  普通客户线程，建议设置不高于osPriorityNormal（1100设计）
 */
osStatus_t osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority);

/**
 * @brief    根据ID获取某线程栈的当前优先级
 * @warning  不能在中断或临界区中调用
 */
osPriority_t osThreadGetPriority (osThreadId_t thread_id);

/**
 * @brief    主动触发一次线程调度，调度完后可能仍然在当前线程执行
 * @warning  不建议使用
 */
osStatus_t osThreadYield (void);

/**
 * @brief    挂起某个线程ID，并触发一次调度
 * @warning  不能在中断或临界区中调用，与osThreadResume配套使用
 */
osStatus_t osThreadSuspend (osThreadId_t thread_id);

/**
 * @brief    恢复某线程ID，并触发一次调度
 * @warning  不能在中断或临界区中调用，不建议使用
 */
osStatus_t osThreadResume (osThreadId_t thread_id);


/**
 * @brief    退出当前线程，若当前线程有全局句柄osThreadId_t，需要在该接口之前赋值NULL
 * @code
    void  main()
    {
	    ............
	    g_mqtttest_handle = NULL;
		osThreadExit();
	}
   @endcode
 * @attention   线程中不能直接return；退出，必须调用该接口进行退出！  
 * @warning  一旦调用该接口后，接口后面的任何代码都不会再被执行到了！！！  
 */
void osThreadExit (void);

/**
 * @brief    强行结束某个指定的线程
 * @param thread_id   [IN] 线程句柄ID
 * @return 结果码
 * @code
    void  main()
    {
	    ............
	    if(g_wdt_TskHandle != NULL)
		{
			osThreadTerminate(g_wdt_TskHandle);
			g_wdt_TskHandle = NULL;
		}
	}
   @endcode
 * @attention   由于操作系统内部不能销毁指定线程的关联资源，请慎用！
 * @warning  异步强行结束指定线程，需要调用者确保调用之前已经释放完该指定线程的堆内存、队列、信号量、互斥量、timer等全局资源，否则会造成系统异常  
 */
osStatus_t osThreadTerminate (osThreadId_t thread_id);

/**
 * @brief    获取当前创建的线程总数
 * @warning  包含挂起等线程
 */
uint32_t osThreadGetCount (void);


//  ==== Thread Flags Functions ====

/**
 * @brief           设置指定线程的标志，此接口可用来替换部分场景下信号量的使用
 * @param thread_id [IN] 线程ID
 * @param flags     [IN] 指定要设置的线程标志，一般设为1
 * @warning         任务通知一般仅用于替换二值信号量和计数型信号量
 * @note            不建议使用！若需要信号量机制，请使用osSemaphoreRelease
 */
uint32_t osThreadFlagsSet (osThreadId_t thread_id, uint32_t flags);

/**
 * @brief    		清除当前线程指定的标志
 * @param flags     [IN] 指定要设置的线程标志，一般设为1
 * @warning  		不能在中断中调用
 * @note     		不建议使用！
 *          
 */
uint32_t osThreadFlagsClear (uint32_t flags);


/**
 * @brief    获取当前线程标志
 * @warning  不能在中断中调用
 * @note     不建议使用！
 */
uint32_t osThreadFlagsGet (void);

/**
 * @brief            在超时时间内等待指定的标志，超时后退出此函数
 * @param flags      [IN] 指定要等待的标志，不能指定最高位的标志，一般设为1
 * @param options    [IN] 指定要等待的标志选项 ，暂时默认为osFlagsWaitAll
 * @param timeout)   [IN] 指定要等待的时间
 * osFlagsWaitAny        0x00000000U ///< Wait for any flag (default).
 * osFlagsWaitAll        0x00000001U ///< Wait for all flag 
 * osFlagsNoClear        0x00000002U ///< Do not clear flags which have been specified to wait for.
 * 可以通过这个选项手动设置线程标志不被清除，否则osThreadFlagsWait将自动清除
 * @code
 * void  
 * {
 * 
 * ....
 *  osThreadFlagsWait (0x00000003U, osFlagsWaitAny |osFlagsNoClear,osWaitForever)
 * }
 * 
 * @warning        不能在中断中调用
 * @attention      线程标志可以被其他线程和中断设置
 * @note           不建议使用！若需要信号量机制，请使用osSemaphoreAcquire
 */
uint32_t osThreadFlagsWait (uint32_t flags, uint32_t options, uint32_t timeout);


//  ==== Generic Wait Functions ====

/**
 * @brief    延迟若干tick后继续执行当前线程，等同于sleep。目前单位为毫秒
 * @warning  1. 不能在中断或临界区中调用 \n
 *           2. 不能在操作系统启动前调用，即只能在线程中调用 \n
 *           3. 不能在idle线程中调用
 */
osStatus_t osDelay (uint32_t ticks);

/**
 * @brief    延迟若干tick后继续执行当前线程，系统内部保证准时调度
 * @warning  会额外增加系统调度负荷，不建议使用  \n
 *           1. 不能在中断或临界区中调用 \n
 *           2. 不能在操作系统启动前调用，即只能在线程中调用 \n
 *           3. 不能在idle线程中调用
 */
osStatus_t osDelayUntil (uint32_t ticks);


//  ==== Timer Management Functions ====

/**
 * @brief   创建一个软件定时器
 * 
 * @param func      [IN] 软件定时器的超时回调函数，该回调函数内不得阻塞，且代码不能太长，以免影响其他timer的运行时序
 * @param type      [IN] 软件定时器的类型，是只超时一次，还是自动重装
 * @param argument  [IN] func回调函数的入参，在func具体实现时，需要通过osTimerGetCallbackArgs接口来获取
 * @param attr    	[IN] 软件定时器的属性，在接口调用之前给结构体赋值
 * typedef struct {
  const char                   *name;   ///< 软件定时器的名称
  uint32_t                 attr_bits;   ///< 默认为0
  void                      *cb_mem;    ///< 默认为NULL
  uint32_t                   cb_size;   ///< 默认为0
} osTimerAttr_t;
 * @return   创建的软件定时器的id，创建失败则返回NULL
 * @warning  系统默认该接口的timer参与standby睡眠唤醒，即会准时从standby唤醒。 \n
 * 如果设置的周期过短，造成standby唤醒过于频繁，可以通过设置osTimerSetLowPowerFlag接口来避免频繁的standby唤醒。 
 * @attention  超时回调函数不得使用阻塞机制，且代码不能运行太长事件，以免影响其他timer的运行时序
 */
osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr);

/**
 * @brief 获取软定时器超时回调函数的参数,与osTimerNew捆绑使用
 * @param timer_id   [IN] 软定时器ID
 * @return 超时回调的参数
 * @attention 创建软定时器是传入的参数，并不会直接传入timer回调函数中，回调函数传入的参数是软定时器句柄，需要通过此句柄获取参数
 */
#define osTimerGetCallbackArgs(timer_id)	(void *) pvTimerGetTimerID((const TimerHandle_t) (timer_id))

/**
 * @brief    根据ID获取timer名
 * @warning  
 */
const char *osTimerGetName (osTimerId_t timer_id);

/**
 * @brief    开启某timer
 * @param timer_id   [IN] 软定时器ID
 * @param ticks      [IN] tick数，目前单位为毫秒
 * @warning  不能在中断或临界区中调用
 * @attention  超时回调函数不得使用阻塞机制，且代码不能运行太长事件，以免影响其他timer的运行时序
 */
osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks);

/**
 * @brief   重新开启某timer
 * @param timer_id   [IN] 软定时器ID
 * @param ticks      [IN] tick数，目前单位为毫秒
 * @warning  不能在中断或临界区中调用
 * @attention  超时回调函数不得使用阻塞机制，且代码不能运行太长事件，以免影响其他timer的运行时序
 */
osStatus_t osTimerRestart (osTimerId_t timer_id);

/**
 * @brief    停止某timer
 * @warning  不能在中断或临界区中调用
 */
osStatus_t osTimerStop (osTimerId_t timer_id);

/**
 * @brief    检测某ID是否正在运行，1表示正在运行；0表示未运行
 * @warning  
 */
uint32_t osTimerIsRunning (osTimerId_t timer_id);


/**
 * @brief    删除某ID
 * @warning  不能在中断或临界区中调用
 */
osStatus_t osTimerDelete (osTimerId_t timer_id);


//  ==== Mutex Management Functions ====

/**
 * @brief    新建一个互斥量，入参建议为osMutexPrioInherit
 * @param attr     [IN] 互斥量属性
 * typedef struct {
  const char                *name;   ///<互斥量名称
  uint32_t                  attr_bits;   ///<设置互斥量属性，1100推荐使用osMutexPrioInherit
                            1、osMutexRecursive:一个线程可以多次使用互斥锁而不锁定自己
                            2、osMutexPrioInherit:具有优先级继承
                            3、osMutexRobust:当线程结束，互斥锁自动释放
                            默认为0时，以上特性都不存在

  void                     *cb_mem;    ///< memory for control block
  uint32_t                   cb_size;   ///< size of provided memory for control block
} osMutexAttr_t;
 * @warning  不能在中断或临界区中调用
 */

osMutexId_t osMutexNew (const osMutexAttr_t *attr);

/**
 * @brief    根据ID获取Mutex名
 * @warning  
 */
const char *osMutexGetName (osMutexId_t mutex_id);


/**
 * @brief    超时等待某互斥量
 * @param  mutex_id  [in]  互斥量ID
 * @param  timeout   [in]  超时tick数，目前单位为毫秒
 * @warning  1. 不能在中断或临界区中调用 \n
 *           2. 不能在操作系统启动前调用，即只能在线程中调用 \n
 *           3. 在idle线程中调用时，超时时间必须为0
 */
osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout);

/**
 * @brief    释放某互斥量
 * @warning  1. 不能在中断或临界区中调用 \n
 *           2. 不能在操作系统启动前调用，即只能在线程中调用
 */
osStatus_t osMutexRelease (osMutexId_t mutex_id);

/**
 * @brief    查询某ID互斥量当前被哪个线程持有
 * @return   NULL表示当前没有线程持有该互斥量
 * @warning  不能在中断或临界区中调用
 */
osThreadId_t osMutexGetOwner (osMutexId_t mutex_id);

/**
 * @brief    删除某互斥量
 * @warning  不能在中断或临界区中调用
 */
osStatus_t osMutexDelete (osMutexId_t mutex_id);


//  ==== Semaphore Management Functions ====

/// Create and Initialize a Semaphore object.
/**
 * @brief    新建一个信号量
 * @param	 max_count      [IN]  信号量的最大个数，为1时即二值信号量，建议设置为0XFFFF 
 * @param	 initial_count  [IN]  信号量的初始值，通常设为0
 * @param	 attr           [IN]  信号量属性，默认为NULL
 * @warning  不能在中断或临界区中调用
 */
osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr);

/**
 * @brief    根据ID获取Semaphorex名
 * @warning  
 */
const char *osSemaphoreGetName (osSemaphoreId_t semaphore_id);

/**
 * @brief    超时等待某信号量
 * @param  mutex_id  [in]  ID
 * @param  timeout   [in]  超时tick数，目前单位为毫秒
 * @warning  1. 在操作系统启动前调用时，超时时间必须为0 \n
 *           2. 在idle线程中调用时，超时时间必须为0
 */
osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout);


/**
 * @brief    释放某信号量
 * @warning  
 */
osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id);


/**
 * @brief    获取某信号量当前的累加数
 * @warning  
 */
uint32_t osSemaphoreGetCount (osSemaphoreId_t semaphore_id);

/**
 * @brief    删除某信号量
 * @warning  不能在中断或临界区中调用
 */
osStatus_t osSemaphoreDelete (osSemaphoreId_t semaphore_id);


//  ==== Message Queue Management Functions ====

/**
 * @brief    新建一个消息队列
 * @param	 msg_count  [IN]  最大消息缓存个数
 * @param	 msg_size   [IN]  每个消息体大小，建议始终设为sizeof(void *)
 * @param	 attr       [IN]  默认为NULL
 * @code
    void  init_user_task()
    {
	    ............
	    g_user_msgQueue_Id = osMessageQueueNew(16, sizeof(void *),NULL);
	}
 * @endcode
 * @note  该消息队列建议传递消息体的地址，需要保证发送方申请消息体内存，接收方释放消息体内存
 * @warning  不能在中断或临界区中调用
 */
osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr);

/**
 * @brief    根据ID获取Queue名
 * @warning  
 */
const char *osMessageQueueGetName (osMessageQueueId_t mq_id);

/**
 * @brief    写一个消息到队列中
 * @param	 mq_id     [IN]  队列ID
 * @param	 msg_ptr   [IN]  消息体的地址，建议使用双指针，队列管道传递的是消息体的地址
 * @param	 msg_prio  [IN]  目前未使用
 * @param	 timeout   [IN]  等待写完成的时间，单位为tick数，目前为毫秒；0表示一旦发现队列满，立即返回错误；osWaitForever表示死等，确保消息入队；建议使用osWaitForever
 * @code
    void  user_send_msg()
    {
		struct user_msg *msg =NULL;

		msg = xy_malloc(sizeof(struct user_msg));
		//set msg param value
		ret = osMessageQueuePut(g_user_msgQueue_Id, (void *)&msg, 0, osWaitForever);
		xy_assert(ret == osOK);
	}
 * @endcode
 * @note  该消息队列建议传递消息体的地址，需要保证发送方申请消息体内存，接收方释放消息体内存
 * @warning  1. 在操作系统启动前调用时，超时时间必须为0 \n
 *           2. 在idle线程中调用时，超时时间必须为0
 */
osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);

/**
 * @brief    超时等待获取一个消息体
 * @param	 mq_id     [IN]  队列ID
 * @param	 msg_ptr   [IN]  消息体的地址，建议使用双指针，队列管道传递的是消息体的地址
 * @param	 msg_prio  [IN]  目前未使用
 * @param	 timeout   [IN]  等待时长，单位为tick数，目前为毫秒；0表示不等待，若无消息，立即返回错误；用户按照需要设置超时等待时长，不建议设为0
 * @code
    void  proc_user_msg_task()
	{
		struct user_msg *msg = NULL;
		while(1) 
		{	
			osMessageQueueGet(g_user_msgQueue_Id, (void *)(&msg), NULL, osWaitForever);
			xy_assert(msg != NULL);
			
			switch(msg->msg_id)
			{
				case :
			}
			xy_free(msg);
			msg = NULL;
		}
	}
 * @endcode
 * @note  该消息队列建议传递消息体的地址，需要保证发送方申请消息体内存，接收方释放消息体内存
 * @warning  1. 在操作系统启动前调用时，超时时间必须为0
 *           2. 在idle线程中调用时，超时时间必须为0
 */
osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);

/**
 * @brief    获取消息队列的深度，即能存储多少条消息
 * @warning  
 */
uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id);

/**
* @brief	获取消息队列的每个消息的长度
* @warning	
*/
uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id);


/**
* @brief	获取消息队列中有多少条消息
* @warning	
*/
uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id);

/**
* @brief	获取消息队列还能存储多少条消息
* @warning	
*/
uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id);

/**
* @brief	清空消息队列中的消息，注意，如果消息队列中的消息是动态申请的地址，不能直接清空，需要手动读出释放内存
* @code
   void xy_queue_clear(osMessageQueueId_t pMsgQueueId)
   {
	   void *elem = NULL;
	   
	   xy_assert (pMsgQueueId != NULL);
   
	   while (osMessageQueueGet(pMsgQueueId, &elem, NULL, 0) == osOK)
	   {
		   xy_free(elem);
	   }
   }
* @endcode

* @warning	不建议使用，若使用，请务必保证消息体堆内存被及时释放
*/
osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id);

/**
* @brief	删除消息队列，如果消息队列内容是动态申请的地址，需要先释放内存
* @code
   void xy_queue_Delete(osMessageQueueId_t pMsgQueueId)
   {
	   void *elem = NULL;
	   
	   xy_assert (pMsgQueueId != NULL);
   
	   while (osMessageQueueGet(pMsgQueueId, &elem, NULL, 0) == osOK)
	   {
		   xy_free(elem);
	   }
	   osMessageQueueDelete(pMsgQueueId);
   }
* @endcode

* @warning	不建议使用，若使用，请务必保证消息体堆内存被及时释放
*/
osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id);


/* ==== Sleep Management Functions ==== */

/**
 * @brief   用于设置进入standby之后，指定线程是否能准时唤醒运行，如osDelay/超时阻塞接口(信号量、互斥量、消息邮箱等)
 *
 * @param thread_id		[IN] 线程ID,NULL表示当前线程
 * @param lpm_flag      [IN] 是否准时唤醒
 * 							osLpmNoRealtime: 线程不会准时唤醒
 * 							osLpmRealtime: 线程会准时唤醒
 * @return   固定返回osOK，此接口设置不会失败
 * @warning  普通线程默认是准时唤醒的，通过该接口可以动态改变线程在standby下准时与否。
 */
osStatus_t osThreadSetLowPowerFlag(osThreadId_t thread_id, osLpmRealtime_t lpm_flag);

/**
 * @brief   获取在进入standby之后，指定线程是否能够准时唤醒
 *
 * @param thread_id		[IN] 线程ID
 * @return   osLpmNoRealtime: 线程不会准时唤醒
 * 			 osLpmRealtime: 线程会准时唤醒
 * @warning  普通线程默认是准时唤醒的，如osDelay/超时阻塞接口(信号量、互斥量、消息邮箱等)。
 */
osLpmRealtime_t osThreadGetLowPowerFlag(osThreadId_t thread_id);

/**
 * @brief   获取在进入standby之前，所有线程中最早的超时时间，即osDelay/超时阻塞接口(信号量、互斥量、消息邮箱等)的最近时间
 *
 * @param sleep_type	[IN] 判断进行睡眠时间统计时，是否关注能够准时唤醒的标志位
 * 							osIgnorLowPowerFlag: 不关注准时唤醒的标志位
 * 							osAttentionLowPowerFlag: 关注准时唤醒的标志位
 * @return   最早的线程超时时间，单位是tick
 * @warning  该接口在睡眠之前计算睡眠时间时调用，用户禁止调用！！！
 */
uint32_t osThreadGetLowPowerTime(osSleepStatisticsType_t sleep_type);

/**
 * @brief   用于设置进入standby之后，指定软定时器是否能准时唤醒运行 
 *
 * @param timer_id      [IN] 软定时器的ID
 * @param lpm_flag      [IN] 是否准时唤醒
 * 							osLpmNoRealtime: 软定时器不会准时唤醒
 * 							osLpmRealtime: 软定时器会准时唤醒
 * @return   固定返回osOK，此接口设置不会失败
 * @warning  默认是准时唤醒的，如果对功耗追求极致，可以梳理所有的timer，对于周期短的可以通过该接口来避免频繁standby唤醒
 */
osStatus_t osTimerSetLowPowerFlag(osTimerId_t timer_id, osLpmRealtime_t lpm_flag);

/**
 * @brief   获取在进入standby之后，指定软定时器是否能够准时唤醒
 *
 * @param timer_id		[IN] 线程ID
 * @return   osLpmNoRealtime: 软定时器不会准时唤醒
 * 			 osLpmRealtime: 软定时器会准时唤醒
 * @warning  该接口用户慎用！必须与芯翼FAE联系。
 */
osLpmRealtime_t osTimerGetLowPowerFlag(osTimerId_t timer_id);

/**
 * @brief   获取在进入standby之前，最早的软定时器超时时间
 *
 * @param sleep_type	[IN] 判断进行睡眠时间统计时，是否关注能够准时唤醒的标志位
 * 							osIgnorLowPowerFlag: 不关注准时唤醒的标志位
 * 							osAttentionLowPowerFlag: 关注准时唤醒的标志位
 * @return   最早的软定时器超时时间，单位是tick
 * @warning  该接口在睡眠之前计算睡眠时间时调用，用户禁止调用！！！
 */
uint32_t osTimerGetLowPowerTime(osSleepStatisticsType_t sleep_type);





/* ==== Memory Management Functions ==== */
/**
* @brief	内存申请相关接口
* @warning	建议客户使用芯翼重定义的宏xy_malloc、xy_malloc_Align、xy_free
*/
/// \param[in]     size         the size to allocate a block of memory with align
/// \return the start address of allocated memory to use
#if( FreeRTOS_HEAP_MALLOC_WITH_RECORD == 1 )
void* osMemoryAllocDebug(size_t size, char *file, int line);
#define osMemoryAlloc(size)    osMemoryAllocDebug(size, __FILE__, __LINE__)
#else
void* osMemoryAlloc(size_t size);
#endif

/// \param[in]     size         the size to allocate a block of memory with align
/// \return the start address of allocated memory to use
#if( FreeRTOS_HEAP_MALLOC_WITH_RECORD == 1 )
void* osMemoryAllocAlignDebug(size_t size, char *file, int line);
#define osMemoryAllocAlign(size)    osMemoryAllocAlignDebug(size, __FILE__, __LINE__)
#else
void* osMemoryAllocAlign(size_t size);
#endif

/// \param[in]     mem          the start address of memory to reallocate
/// \param[in]     size         the size to allocate a block of memory
/// \return the start address of reallocated memory to use
#if( FreeRTOS_HEAP_MALLOC_WITH_RECORD == 1 )
void* osMemoryReallocDebug(void *mem, size_t size, char *file, int line);
#define osMemoryRealloc(mem, size)    osMemoryReallocDebug(mem, size, __FILE__, __LINE__)
#else
void* osMemoryRealloc(void *mem, size_t size);
#endif

/// \param[in]     mem          the start address of memory to reallocate
/// \param[in]     size         the size to allocate a block of memory
/// \return the start address of reallocated memory to use
#if( FreeRTOS_HEAP_MALLOC_WITH_RECORD == 1 )
void* osMemoryReallocAlignDebug(void *mem, size_t size, char *file, int line);
#define osMemoryReallocAlign(mem, size)    osMemoryReallocDebug(mem, size, __FILE__, __LINE__)
#else
void* osMemoryReallocAlign(void *mem, size_t size);
#endif

/// \param[in]     mem          the start address of memory to free
/// \return status code that indicates the execution status of the function.
#if( FreeRTOS_HEAP_FREE_WITH_RECORD == 1 )
osStatus_t osMemoryFreeDebug(void *mem, char *file, int line);
#define osMemoryFree(mem)    osMemoryFreeDebug(mem, __FILE__, __LINE__)
#else
osStatus_t osMemoryFree(void *mem);
#endif


#endif  /* __OSAL_H_ */
