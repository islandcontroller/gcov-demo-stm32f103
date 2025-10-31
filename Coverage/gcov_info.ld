SECTIONS
{
  /* gcov info for each instrumented unit */
  .gcov_info (READONLY):
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__gcov_info_start = .);
    KEEP (*(.gcov_info*))
    PROVIDE_HIDDEN(__gcov_info_end = .);
    . = ALIGN(4);
  } >FLASH
}