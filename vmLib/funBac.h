
/*
 * Copyright 2011 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: funBac.h
 */


#if defined(RTS_CFG_BACNET)

									BAC_FindObject					,	/* 230 */
									BAC_GetDevState 				,	/* 231 */
									BAC_GetObjState 				,	/* 232 */
									BAC_GetObjPrio					,	/* 233 */
									BAC_SetObjPrio					,	/* 234 */
									BAC_GetLocDeviceID				,	/* 235 */
									BAC_GP_Boolean					,	/* 236 */
									BAC_GP_Enumerated				,	/* 237 */
									BAC_GP_CharacterString			,	/* 238 */
									BAC_GP_Unsigned 				,	/* 239 */
									BAC_GP_Real 					,	/* 240 */
									BAC_GP_Bitstring				,	/* 241 */
									BAC_GP_DateTime 				,	/* 242 */
									BAC_GP_StatusFlags				,	/* 243 */
									BAC_GP_LimitEnable				,	/* 244 */
									BAC_GP_EventTransition			,	/* 245 */
									BAC_GP_Any						,	/* 246 */
									BAC_GP_DateRange				,	/* 247 */
									BAC_GP_DevObjPropReference		,	/* 248 */
									BAC_GPA_Unsigned				,	/* 249 */
									BAC_GPA_Enumerated				,	/* 250 */
									BAC_GPA_Real					,	/* 251 */
									BAC_GPA_CharacterString 		,	/* 252 */
									BAC_GPA_TimeStamp				,	/* 253 */
									BAC_GPA_DevObjPropReference 	,	/* 254 */
									BAC_GPA_DailySchedule			,	/* 255 */
									BAC_GPA_SpecialEvent			,	/* 256 */
									BAC_GP_Ex_Boolean				,	/* 257 */
									BAC_GP_Ex_Enumerated			,	/* 258 */
									BAC_GP_Ex_CharacterString		,	/* 259 */
									BAC_GP_Ex_Unsigned				,	/* 260 */
									BAC_GP_Ex_Real					,	/* 261 */
									BAC_GP_Ex_Bitstring 			,	/* 262 */
									BAC_GP_Ex_DateTime				,	/* 263 */
									BAC_GP_Ex_StatusFlags			,	/* 264 */
									BAC_GP_Ex_LimitEnable			,	/* 265 */
									BAC_GP_Ex_EventTransition		,	/* 266 */
									BAC_GP_Ex_Any					,	/* 267 */
									BAC_GP_Ex_DateRange 			,	/* 268 */
									BAC_GP_Ex_DevObjPropReference	,	/* 269 */
									BAC_GPA_Ex_Unsigned 			,	/* 270 */
									BAC_GPA_Ex_Enumerated			,	/* 271 */
									BAC_GPA_Ex_Real 				,	/* 272 */
									BAC_GPA_Ex_CharacterString		,	/* 273 */
									BAC_GPA_Ex_TimeStamp			,	/* 274 */
									BAC_GPA_Ex_DevObjPropReference	,	/* 275 */
									BAC_GPA_Ex_DailySchedule		,	/* 276 */
									BAC_GPA_Ex_SpecialEvent 		,	/* 277 */
									BAC_SP_Boolean					,	/* 278 */
									BAC_SP_Enumerated				,	/* 279 */
									BAC_SP_CharacterString			,	/* 280 */
									BAC_SP_Unsigned 				,	/* 281 */
									BAC_SP_Real 					,	/* 282 */
									BAC_SP_Bitstring				,	/* 283 */
									BAC_SP_DateTime 				,	/* 284 */
									BAC_SP_LimitEnable				,	/* 285 */
									BAC_SP_EventTransition			,	/* 286 */
									BAC_SP_Any						,	/* 287 */
									BAC_SP_DateRange				,	/* 288 */
									BAC_SP_DevObjPropReference		,	/* 289 */
									BAC_SPA_Unsigned				,	/* 290 */
									BAC_SPA_CharacterString 		,	/* 291 */
									BAC_SPA_DevObjPropReference 	,	/* 292 */
									BAC_SPA_DailySchedule			,	/* 293 */
									BAC_SPA_SpecialEvent			,	/* 294 */
									BAC_SP_Ex_Boolean				,	/* 295 */
									BAC_SP_Ex_Enumerated			,	/* 296 */
									BAC_SP_Ex_CharacterString		,	/* 297 */
									BAC_SP_Ex_Unsigned				,	/* 298 */
									BAC_SP_Ex_Real					,	/* 299 */
									BAC_SP_Ex_Bitstring 			,	/* 300 */
									BAC_SP_Ex_DateTime				,	/* 301 */
									BAC_SP_Ex_LimitEnable			,	/* 302 */
									BAC_SP_Ex_EventTransition		,	/* 303 */
									BAC_SP_Ex_Any					,	/* 304 */
									BAC_SP_Ex_DateRange 			,	/* 305 */
									BAC_SP_Ex_DevObjPropReference	,	/* 306 */
									BAC_SPA_Ex_Unsigned 			,	/* 307 */
									BAC_SPA_Ex_CharacterString		,	/* 308 */
									BAC_SPA_Ex_DevObjPropReference	,	/* 309 */
									BAC_SPA_Ex_DailySchedule		,	/* 310 */
									BAC_SPA_Ex_SpecialEvent 		,	/* 311 */
									BAC_SubscribeProperty			,	/* 312 */
									BAC_UnSubscribeProperty 		,	/* 313 */
									BAC_EventSubscribe				,	/* 314 */
									BAC_EventUnSubscribe			,	/* 315 */
									BAC_EventGet					,	/* 316 */
									BAC_EventGetString				,	/* 317 */
									BAC_EventAcknowledge			,	/* 318 */
									BAC_GetConfigRes				,	/* 319 */
									BAC_GetDevTrans 				,	/* 320 */
									BAC_GetLogBuffer				,	/* 321 */
									BAC_ReinitializeDev 			,	/* 322 */
									_BAC_ReinitializeDev			,	/* 323 */
									_BAC_DevCommControl 			,	/* 324 */
									_BAC_ReadFile					,	/* 325 */
									_BAC_WriteFile					,	/* 326 */
									BAC_SPV_Enumerated				,	/* 327 */
									BAC_SPV_Unsigned				,	/* 328 */
									BAC_SPV_Real					,	/* 329 */
									BAC_SPV_Ex_Enumerated			,	/* 330 */
									BAC_SPV_Ex_Unsigned 			,	/* 331 */
									BAC_SPV_Ex_Real 				,	/* 332 */
									NULL							,	/* 333 */
									NULL							,	/* 334 */
									NULL							,	/* 335 */
									NULL							,	/* 336 */
									NULL							,	/* 337 */
									NULL							,	/* 338 */
									NULL							,	/* 339 */
		
		
#else	/* RTS_CFG_BACNET */

									NULL							,	/* 230 */
									NULL							,	/* 231 */
									NULL							,	/* 232 */
									NULL							,	/* 233 */
									NULL							,	/* 234 */
									NULL							,	/* 235 */
									NULL							,	/* 236 */
									NULL							,	/* 237 */
									NULL							,	/* 238 */
									NULL							,	/* 239 */
									NULL							,	/* 240 */
									NULL							,	/* 241 */
									NULL							,	/* 242 */
									NULL							,	/* 243 */
									NULL							,	/* 244 */
									NULL							,	/* 245 */
									NULL							,	/* 246 */
									NULL							,	/* 247 */
									NULL							,	/* 248 */
									NULL							,	/* 249 */
									NULL							,	/* 250 */
									NULL							,	/* 251 */
									NULL							,	/* 252 */
									NULL							,	/* 253 */
									NULL							,	/* 254 */
									NULL							,	/* 255 */
									NULL							,	/* 256 */
									NULL							,	/* 257 */
									NULL							,	/* 258 */
									NULL							,	/* 259 */
									NULL							,	/* 260 */
									NULL							,	/* 261 */
									NULL							,	/* 262 */
									NULL							,	/* 263 */
									NULL							,	/* 264 */
									NULL							,	/* 265 */
									NULL							,	/* 266 */
									NULL							,	/* 267 */
									NULL							,	/* 268 */
									NULL							,	/* 269 */
									NULL							,	/* 270 */
									NULL							,	/* 271 */
									NULL							,	/* 272 */
									NULL							,	/* 273 */
									NULL							,	/* 274 */
									NULL							,	/* 275 */
									NULL							,	/* 276 */
									NULL							,	/* 277 */
									NULL							,	/* 278 */
									NULL							,	/* 279 */
									NULL							,	/* 280 */
									NULL							,	/* 281 */
									NULL							,	/* 282 */
									NULL							,	/* 283 */
									NULL							,	/* 284 */
									NULL							,	/* 285 */
									NULL							,	/* 286 */
									NULL							,	/* 287 */
									NULL							,	/* 288 */
									NULL							,	/* 289 */
									NULL							,	/* 290 */
									NULL							,	/* 291 */
									NULL							,	/* 292 */
									NULL							,	/* 293 */
									NULL							,	/* 294 */
									NULL							,	/* 295 */
									NULL							,	/* 296 */
									NULL							,	/* 297 */
									NULL							,	/* 298 */
									NULL							,	/* 299 */
									NULL							,	/* 300 */
									NULL							,	/* 301 */
									NULL							,	/* 302 */
									NULL							,	/* 303 */
									NULL							,	/* 304 */
									NULL							,	/* 305 */
									NULL							,	/* 306 */
									NULL							,	/* 308 */
									NULL							,	/* 309 */
									NULL							,	/* 310 */
									NULL							,	/* 311 */
									NULL							,	/* 312 */
									NULL							,	/* 313 */
									NULL							,	/* 314 */
									NULL							,	/* 315 */
									NULL							,	/* 316 */
									NULL							,	/* 318 */
									NULL							,	/* 319 */
									NULL							,	/* 320 */
									NULL							,	/* 321 */
									NULL							,	/* 322 */
									NULL							,	/* 323 */
									NULL							,	/* 324 */
									NULL							,	/* 325 */
									NULL							,	/* 326 */
									NULL							,	/* 327 */
									NULL							,	/* 328 */
									NULL							,	/* 329 */
									NULL							,	/* 330 */
									NULL							,	/* 331 */
									NULL							,	/* 332 */
									NULL							,	/* 333 */
									NULL							,	/* 334 */
									NULL							,	/* 335 */
									NULL							,	/* 336 */
									NULL							,	/* 337 */
									NULL							,	/* 338 */
									NULL							,	/* 339 */
#endif	/* RTS_CFG_BACNET */

/* ---------------------------------------------------------------------------- */

