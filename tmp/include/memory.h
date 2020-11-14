/* memory.h - roundmb, truncmb, freestk */

#define	PAGE_SIZE	4096
#define	HOLESTART	((char *)(640 * 1024))
#define	HOLEEND		((char *)(1024 * 1024))

/*----------------------------------------------------------------------
 * roundmb, truncmb - Round or truncate address to memory block size
 *----------------------------------------------------------------------
 */
#define	roundmb(x)	(char *)( (7 + (uint32)(x)) & (~7) )
#define	truncmb(x)	(char *)( ((uint32)(x)) & (~7) )
#define roundpage(x)	(char *)( ( 4095 + (uint32)(x) ) & (~4095) )
#define truncpage(x)	(char *)( ((uint32)(x)) & (~4095) ) 


/* Page Table offset, Page Base Offset, Page Offset */

#define page_offset(x)  	(uint32)( (uint32)(x) & 4095 )
#define page_table_offset(x)	(uint32)( ((uint32)x>>12) & 1023 ) 
#define page_base_offset(x)	(uint32)( ((uint32)x>>22) & 1023 )
#define physical_frame(x)	(uint32)( ((uint32)x>>12) & 0xFFFFF)
/*----------------------------------------------------------------------
 *  freestk  --  Free stack memory allocated by getstk
 *----------------------------------------------------------------------
 */
#define	freestk(p,len)	freemem((char *)((uint32)(p)		\
				- ((uint32)roundmb(len))	\
				+ (uint32)sizeof(uint32)),	\
				(uint32)roundmb(len) )

struct	memblk	{			/* See roundmb & truncmb	*/
	struct	memblk	*mnext;		/* Ptr to next free memory blk	*/
	uint32	mlength;		/* Size of blk (includes memblk)*/
	};

extern	struct	memblk	memlist;	/* Head of free memory list	*/
extern  struct  memblk  ffs_memlist;	/* List of free pages in FFS area	*/
extern  struct  memblk  pt_memlist;	/* List of free pages in PT PD area	*/


extern	void	*minheap;		/* Start of heap		*/
extern	void	*maxheap;		/* Highest valid heap address	*/
extern 	void	*ffs_start;		/* Start of heap address	*/
extern	void	*ffs_end;		/* End of heap address			*/
extern	void 	*pdpt_start;		/* pdpt start address			*/
extern	void 	*pdpt_end;		/* pdpt end address			*/
extern 	struct pd_t* PD_Base_System;	/* GLobal Variable to be used with system process : Contains PDBR */
extern 	struct pd_t* PD_Base_Kernel;	/* GLobal Variable to be used with kernel process : Contains PDBR */


/* Added by linker */

extern	int	text;			/* Start of text segment	*/
extern	int	etext;			/* End of text segment		*/
extern	int	data;			/* Start of data segment	*/
extern	int	edata;			/* End of data segment		*/
extern	int	bss;			/* Start of bss segment		*/
extern	int	ebss;			/* End of bss segment		*/
extern	int	end;			/* End of program		*/
