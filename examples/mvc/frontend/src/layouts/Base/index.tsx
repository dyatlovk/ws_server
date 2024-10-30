import styles from './style.module.scss'
import MainMenu from '@components/MainMenu/index'
import { Outlet } from 'react-router-dom'

const Base = (): JSX.Element => {
  return (
    <div className={styles.wrapper}>
      <MainMenu />
      <Outlet />
    </div>
  )
}

export default Base
